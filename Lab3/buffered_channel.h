#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_
#include <condition_variable>
#include <mutex>
#include <queue>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size_(size) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mutex_);

        full_.wait(lock, [&]
        {
            return buffer_.size() < buffer_size_ || is_closed_;
        });

        if (is_closed_) {
            throw std::runtime_error("Channel is closed");
        }

        buffer_.push(std::move(value));
        empty_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mutex_);
        empty_.wait(lock, [this]() {
            return !buffer_.empty() || (is_closed_ && buffer_.empty());
        });

        if (!buffer_.empty()) {
            T value = std::move(buffer_.front());
            buffer_.pop();
            full_.notify_one();
            return {std::move(value), true};
        }
        return {T(), false};
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mutex_);
        is_closed_ = true;
        empty_.notify_all();
        full_.notify_all();
    }

private:
    std::queue<T> buffer_;
    std::mutex mutex_;
    int buffer_size_;
    std::condition_variable full_;
    std::condition_variable empty_;

    bool is_closed_ = false;
};

#endif // BUFFERED_CHANNEL_H_