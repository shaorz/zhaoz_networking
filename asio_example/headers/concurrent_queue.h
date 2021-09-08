#pragma once

#include "net_common.h"

namespace olc {
	namespace net {

		template<typename T> class concurrent_queue {
		private:

		protected:
			std::mutex mutex_queue;
			std::deque<T> data_queue;

			std::condition_variable cvBlocking;
			std::mutex muxBlocking;

		public:
			concurrent_queue() = default;
			concurrent_queue(const concurrent_queue<T>&) = delete;

			virtual ~concurrent_queue() {
				clear();
			}

			// reading from front
			const T& front() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				return data_queue.front();
			}

			// reading from back
			const T& back() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				return data_queue.back();
			}

			//read and remove from front
			T pop_front() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				// is it returning a rvalue reference
				auto t = std::move(data_queue.front());
				data_queue.pop_front();
				return t;
			}

			// read and remove from back
			T pop_back() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				// is it returning a rvalue reference
				auto t = std::move(data_queue.back());
				data_queue.pop_back();
				return t;
			}

			// push to the front
			void push_front(const T& item) {
				std::unique_lock<std::mutex> lock(mutex_queue);
				// data_queue.push_front(item);
				data_queue.emplace_front(std::move(item));

				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			void push_back(const T& item) {
				std::unique_lock<std::mutex> lock(mutex_queue);
				//data_queue.push_back(item);
				data_queue.emplace_back(std::move(item));
				
				std::unique_lock<std::mutex> ul(muxBlocking);
				cvBlocking.notify_one();
			}

			bool empty() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				return data_queue.empty();
			}

			uint32_t count() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				return data_queue.size();
			}

			void clear() {
				std::unique_lock<std::mutex> lock(mutex_queue);
				data_queue.clear();
			}

			void wait() {
				while (empty()) {
					std::unique_lock<std::mutex> ul(muxBlocking);
					cvBlocking.wait(ul);
				}
			}

 		};
	
	
	
	}
}