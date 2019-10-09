//
// Created by kelleycd on 4/16/19.
//

#include "SafeQueue.h"
#include "message.h"

template <typename T>

SafeQueue<T>::SafeQueue()
{
    sem_init(&semaphore, 0, 0);
}

template <typename T>
T SafeQueue<T>::dequeue()
{
    T message{};
    sem_wait(&semaphore);
    std::lock_guard<std::mutex> lock(mutex);
    message = queue.front();
    queue.pop();
    return message;

}

template <typename T>
void SafeQueue<T>::enqueue(T message)
{
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(message);
    sem_post(&semaphore);
}