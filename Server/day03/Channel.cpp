#include "Channel.h"

Channel::Channel(Epoll* _ep, int _fd) : ep(_ep), fd(_fd), events(0), revents(0), inEpoll(false) {}

Channel::~Channel() {}

int Channel::getFd() {
    return fd;
}

uint32_t Channel::getEvents() {
    return events;
}

uint32_t Channel::getRevents() {
    return revents;
}

bool Channel::getInEpoll() {
    return inEpoll;
}

void Channel::setInEpoll() {
    inEpoll = true;
}

void Channel::setRevents(uint32_t _ev) {
    revents = _ev;
}