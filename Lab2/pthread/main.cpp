#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <pthread.h>
#include <thread>

using namespace std;
using Matrix = vector<vector<int64_t>>;

mt19937 rng(chrono::high_resolution_clock::now().time_since_epoch().count());

Matrix makeRandomMatrix(size_t n, int64_t range)
{
    Matrix mat(n, vector<int64_t>(n));
    uniform_int_distribution<int64_t> dist(-range + 1, range - 1);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            mat[i][j] = dist(rng);
        }
    }
    return mat;
}

Matrix naiveMultiply(const Matrix& X, const Matrix& Y)
{
    if (X.empty() || Y.empty() || X[0].size() != Y.size()) {
        throw runtime_error("invalid matrix dimensions");
    }

    size_t rows = X.size();
    size_t cols = Y[0].size();
    size_t inner = X[0].size();

    Matrix result(rows, vector<int64_t>(cols, 0));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            int64_t dot = 0;
            for (size_t k = 0; k < inner; ++k) {
                dot += X[i][k] * Y[k][j];
            }
            result[i][j] = dot;
        }
    }

    return result;
}

class BlockScheduler {
    pthread_mutex_t lock;
    int current_i;
    int current_j;
    int total_blocks;

public:
    BlockScheduler(int blocks)
        : current_i(0), current_j(0), total_blocks(blocks)
    {
        pthread_mutex_init(&lock, nullptr);
    }

    ~BlockScheduler() {
        pthread_mutex_destroy(&lock);
    }

    bool getNextBlock(int& bi, int& bj) {
        pthread_mutex_lock(&lock);
        bool has_work = current_i < total_blocks;
        if (has_work) {
            bi = current_i;
            bj = current_j;

            ++current_j;
            if (current_j >= total_blocks) {
                current_j = 0;
                ++current_i;
            }
        }
        pthread_mutex_unlock(&lock);
        return has_work;
    }
};

struct ThreadContext {
    const Matrix* mat1;
    const Matrix* mat2;
    Matrix* result;
    int dimension;
    int chunk;
    BlockScheduler* scheduler;
};

void* computeBlocks(void* ctx_ptr) {
    ThreadContext* ctx = static_cast<ThreadContext*>(ctx_ptr);
    int bi, bj;

    while (ctx->scheduler->getNextBlock(bi, bj)) {
        int row_start = bi * ctx->chunk;
        int row_end = min(row_start + ctx->chunk, ctx->dimension);
        int col_start = bj * ctx->chunk;
        int col_end = min(col_start + ctx->chunk, ctx->dimension);

        for (int r = row_start; r < row_end; ++r) {
            for (int c = col_start; c < col_end; ++c) {
                int64_t accum = 0;
                for (int k = 0; k < ctx->dimension; ++k) {
                    accum += (*ctx->mat1)[r][k] * (*ctx->mat2)[k][c];
                }
                (*ctx->result)[r][c] = accum;
            }
        }
    }

    return nullptr;
}

class ParallelMatMul {
    const Matrix& left;
    const Matrix& right;
    Matrix& output;
    int size;
    int tile;

public:
    ParallelMatMul(const Matrix& L, const Matrix& R, Matrix& Out, int tile_size)
        : left(L), right(R), output(Out),
          size(static_cast<int>(L.size())), tile(tile_size) {}

    void runWithThreads(int thread_count) {
        int blocks = (size + tile - 1) / tile;
        BlockScheduler scheduler(blocks);

        ThreadContext ctx;
        ctx.mat1 = &left;
        ctx.mat2 = &right;
        ctx.result = &output;
        ctx.dimension = size;
        ctx.chunk = tile;
        ctx.scheduler = &scheduler;

        vector<pthread_t> workers(thread_count);

        for (int i = 0; i < thread_count; ++i) {
            pthread_create(&workers[i], nullptr, computeBlocks, &ctx);
        }

        for (int i = 0; i < thread_count; ++i) {
            pthread_join(workers[i], nullptr);
        }
    }
};

int main() {
    const int DIM = 1000;
    const int64_t VALUE_RANGE = 100000;

    cout << "Creating " << DIM << "x" << DIM << " matrices..." << endl;

    Matrix matA = makeRandomMatrix(DIM, VALUE_RANGE);
    Matrix matB = makeRandomMatrix(DIM, VALUE_RANGE);

    Matrix seq_result;
    Matrix par_result(DIM, vector<int64_t>(DIM, 0));

    cout << "Single-threaded computation..." << endl;
    auto t1 = chrono::high_resolution_clock::now();
    seq_result = naiveMultiply(matA, matB);
    auto t2 = chrono::high_resolution_clock::now();
    auto seq_time = chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

    cout << "Baseline time: " << seq_time << " μs" << endl;
    cout << string(70, '=') << endl;

    unsigned int cores = thread::hardware_concurrency();
    if (cores == 0) cores = 4;

    cout << "System cores: " << cores << endl;
    cout << string(70, '-') << endl;
    cout << setw(12) << "Tile"
         << setw(14) << "Tasks"
         << setw(18) << "Parallel μs"
         << setw(12) << "Gain" << endl;
    cout << string(70, '-') << endl;

    vector<int> tile_sizes = {8, 16, 32, 64, 128, 256, 512, 1024};

    for (int ts : tile_sizes) {
        if (ts > DIM) continue;

        for (auto& row : par_result) {
            fill(row.begin(), row.end(), 0);
        }

        ParallelMatMul multiplier(matA, matB, par_result, ts);

        auto start = chrono::high_resolution_clock::now();
        multiplier.runWithThreads(cores);
        auto finish = chrono::high_resolution_clock::now();
        auto par_time = chrono::duration_cast<chrono::microseconds>(finish - start).count();

        int grid = (DIM + ts - 1) / ts;
        int tasks = grid * grid;
        double speedup = static_cast<double>(seq_time) / (par_time + 1);

        cout << setw(12) << ts
             << setw(14) << tasks
             << setw(18) << par_time
             << setw(12) << fixed << setprecision(2) << speedup << endl;
    }

    return 0;
}