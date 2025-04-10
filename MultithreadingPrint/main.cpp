#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtx;
condition_variable cv;
int current_num = 1;  // 当前应打印的数字
int turn = 0;         // 控制线程轮流执行，0、1、2分别对应三个线程

void print_number(int id) {
    while (true) {
        unique_lock<mutex> lock(mtx);
        // 等待条件满足：轮到当前线程或数字已超过100
        cv.wait(lock, [id]() { return turn == id || current_num > 100; });

        if (current_num > 100) {
            lock.unlock();
            cv.notify_all();  // 唤醒所有线程以退出
            break;
        }

        // 打印当前数字并递增
        cout << current_num << endl;
        current_num++;

        // 更新下一个应执行的线程
        turn = (turn + 1) % 3;

        lock.unlock();
        cv.notify_all();  // 通知其他线程检查条件
    }
}

int main() {
    thread t0(print_number, 0);
    thread t1(print_number, 1);
    thread t2(print_number, 2);

    t0.join();
    t1.join();
    t2.join();

    return 0;
}