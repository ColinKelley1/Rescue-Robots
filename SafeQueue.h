//
// Created by kelleycd on 4/16/19.
//

#ifndef HW4SERVER_SAFEQUEUE_H
#define HW4SERVER_SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <semaphore.h>
#include "message.h"


template <typename T>

class SafeQueue
{
private:
    std::queue<T> queue;
    std::mutex mutex;
    sem_t semaphore;

public:
    SafeQueue();
    T dequeue();
    void enqueue(T m);

};
#endif //HW4SERVER_SAFEQUEUE_H
