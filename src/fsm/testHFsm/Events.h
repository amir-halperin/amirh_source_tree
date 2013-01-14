#pragma once

#include <fsm/Event.h>

class AdminUpEvent : public fsm::Event<AdminUpEvent>
{
public:
  AdminUpEvent() : Event("AdminUpEvent") {}
};

class AdminDownEvent : public fsm::Event<AdminDownEvent>
{
public:
  AdminDownEvent() : Event("AdminDownEvent") {}
};

class UpEvent : public fsm::Event<UpEvent>
{
public:
  UpEvent() : Event("UpEvent") {}
};

class DownEvent : public fsm::Event<DownEvent>
{
public:
  DownEvent() : Event("DownEvent") {}
};