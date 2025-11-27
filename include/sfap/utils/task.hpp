#pragma once

#include <coroutine>
#include <exception>
#include <type_traits>
#include <utility>

namespace sfap {

template <typename T> class task {
  public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    task() noexcept = default;

    explicit task(handle_type h) noexcept : h_(h) {}

    task(const task&) = delete;
    task& operator=(const task&) = delete;

    task(task&& other) noexcept : h_(other.h_) {
        other.h_ = {};
    }

    task& operator=(task&& other) noexcept {
        if (this != &other) {
            if (h_) {
                h_.destroy();
            }
            h_ = other.h_;
            other.h_ = {};
        }
        return *this;
    }

    ~task() {
        if (h_) {
            h_.destroy();
        }
    }

    bool valid() const noexcept {
        return static_cast<bool>(h_);
    }

    auto operator co_await() noexcept {
        struct awaiter {
            handle_type h;

            bool await_ready() const noexcept {
                return !h || h.done();
            }

            void await_suspend(std::coroutine_handle<> cont) noexcept {
                h.promise().continuation = cont;
                h.resume();
            }

            T await_resume() noexcept {
                return std::move(h.promise().value);
            }
        };

        return awaiter{h_};
    }

    void start_detached() noexcept {
        if (h_) {
            h_.resume();
        }
    }

    struct promise_type {
        T value{};
        std::coroutine_handle<> continuation;

        task get_return_object() noexcept {
            return task{handle_type::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        auto final_suspend() noexcept {
            struct final_awaitable {
                bool await_ready() const noexcept {
                    return false;
                }

                void await_suspend(handle_type h) noexcept {
                    auto cont = h.promise().continuation;
                    if (cont) {
                        cont.resume();
                    }
                }

                void await_resume() noexcept {}
            };

            return final_awaitable{};
        }

        void unhandled_exception() noexcept {
            std::terminate();
        }

        template <typename U>
            requires std::convertible_to<U, T>
        void return_value(U&& v) noexcept(std::is_nothrow_assignable_v<T&, U&&>) {
            value = std::forward<U>(v);
        }
    };

  private:
    handle_type h_{};
};

template <> class task<void> {
  public:
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    task() noexcept = default;

    explicit task(handle_type h) noexcept : h_(h) {}

    task(const task&) = delete;
    task& operator=(const task&) = delete;

    task(task&& other) noexcept : h_(other.h_) {
        other.h_ = {};
    }

    task& operator=(task&& other) noexcept {
        if (this != &other) {
            if (h_) {
                h_.destroy();
            }
            h_ = other.h_;
            other.h_ = {};
        }
        return *this;
    }

    ~task() {
        if (h_) {
            h_.destroy();
        }
    }

    bool valid() const noexcept {
        return static_cast<bool>(h_);
    }

    auto operator co_await() noexcept {
        struct awaiter {
            handle_type h;

            bool await_ready() const noexcept {
                return !h || h.done();
            }

            void await_suspend(std::coroutine_handle<> cont) noexcept {
                h.promise().continuation = cont;
                h.resume();
            }

            void await_resume() noexcept {}
        };

        return awaiter{h_};
    }

    void start_detached() noexcept {
        if (h_) {
            h_.resume();
        }
    }

    struct promise_type {
        std::coroutine_handle<> continuation;

        task get_return_object() noexcept {
            return task{handle_type::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept {
            return {};
        }

        auto final_suspend() noexcept {
            struct final_awaitable {
                bool await_ready() const noexcept {
                    return false;
                }

                void await_suspend(handle_type h) noexcept {
                    auto cont = h.promise().continuation;
                    if (cont) {
                        cont.resume();
                    }
                }

                void await_resume() noexcept {}
            };

            return final_awaitable{};
        }

        void unhandled_exception() noexcept {
            std::terminate();
        }

        void return_void() noexcept {}
    };

  private:
    handle_type h_{};
};

} // namespace sfap