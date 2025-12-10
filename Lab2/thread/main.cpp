#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <random>
#include <cstdint>
#include <algorithm>
#include <iomanip>

using namespace std;
using Matrix = vector<vector<int64_t>>;

mt19937 gen(std::chrono::system_clock::now().time_since_epoch().count());

Matrix GenerateRandomMatrix(size_t size, int64_t mod)
{
    Matrix ans(size, vector<int64_t>(size, 0));
    uniform_int_distribution<int64_t> dist(-mod + 1, mod - 1);

    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            ans[i][j] = dist(gen);
        }
    }
    return ans;
}

Matrix SimpleMultiplication(const Matrix& A, const Matrix& B)
{
    if (A.empty() || B.empty() || A[0].size() != B.size()) {
        throw std::invalid_argument("Matrixes can't be multiplied");
    }
    size_t n = A.size();
    size_t m = B[0].size();
    size_t l = A[0].size();

    Matrix ans(n, vector<int64_t>(m, 0));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < m; j++) {
            for (size_t k = 0; k < l; k++) {
                ans[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return ans;
}

class MatrixMultiplierThread {
private:
    const Matrix& A;
    const Matrix& B;
    Matrix& C;
    int block_size;
    int N;
    int M;
    int K;

public:
    MatrixMultiplierThread(const Matrix& A, const Matrix& B, Matrix& C, int block_size)
        : A(A), B(B), C(C), block_size(block_size),
          N(A.size()), M(B[0].size()), K(A[0].size()) {}

    void multiplyBlock(int start_row, int end_row, int start_col, int end_col) {
        for (int i = start_row; i < end_row; ++i) {
            for (int j = start_col; j < end_col; ++j) {
                int64_t sum = 0;
                for (int k = 0; k < K; ++k) {
                    sum += A[i][k] * B[k][j];
                }
                C[i][j] = sum;
            }
        }
    }

    void parallelMultiply() {
        int num_blocks_row = (N + block_size - 1) / block_size;
        int num_blocks_col = (M + block_size - 1) / block_size;

        std::vector<std::thread> threads;
        threads.reserve(num_blocks_row * num_blocks_col);

        for (int i = 0; i < N; i += block_size) {
            for (int j = 0; j < M; j += block_size) {
                int start_row = i;
                int end_row = std::min(i + block_size, N);
                int start_col = j;
                int end_col = std::min(j + block_size, M);

                threads.emplace_back(&MatrixMultiplierThread::multiplyBlock, this,
                                   start_row, end_row, start_col, end_col);
            }
        }

        for (auto& thread : threads) {
            if(thread.joinable())
                thread.join();
        }
    }
};

int main() {
    const int N = 1000;
    const int64_t MOD = 100000;

    cout << "Generating matrices " << N << "x" << N << "..." << endl;
    auto A = GenerateRandomMatrix(N, MOD);
    auto B = GenerateRandomMatrix(N, MOD);
    Matrix C_seq;
    Matrix C_par(N, vector<int64_t>(N));

    cout << "Running sequential multiplication..." << endl;
    auto start_seq = chrono::high_resolution_clock::now();
    C_seq = SimpleMultiplication(A, B);
    auto end_seq = chrono::high_resolution_clock::now();
    auto duration_seq = chrono::duration_cast<chrono::microseconds>(end_seq - start_seq).count();

    cout << "Sequential Time: " << duration_seq << " mcs" << endl;
    cout << string(80, '-') << endl;
    cout << setw(10) << "Block Size"
         << setw(15) << "Threads"
         << setw(20) << "Time (mcs)"
         << setw(15) << "Speedup" << endl;
    cout << string(80, '-') << endl;

    vector<int> block_sizes = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

    for (int bs : block_sizes) {
        if (bs > N) {
            break;
        }

        for(auto &row : C_par) fill(row.begin(), row.end(), 0);

        MatrixMultiplierThread multiplier(A, B, C_par, bs);

        auto start_par = chrono::high_resolution_clock::now();
        multiplier.parallelMultiply();
        auto end_par = chrono::high_resolution_clock::now();
        auto duration_par = chrono::duration_cast<chrono::microseconds>(end_par - start_par).count();

        int blocks_per_dim = (N + bs - 1) / bs;
        int total_threads = blocks_per_dim * blocks_per_dim;
        double speedup = (double)duration_seq / (duration_par + 1);

        cout << setw(10) << bs
             << setw(15) << total_threads
             << setw(20) << duration_par
             << setw(15) << fixed << setprecision(2) << speedup << endl;
    }

    return 0;
}