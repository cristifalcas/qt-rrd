#ifndef PAGES_H_INCLUDED
#define PAGES_H_INCLUDED

#include "commons.h"

#include <QWidget>

class Pages : public QWidget
{
public:
    virtual void draw() = 0;

private:
    virtual void connects() = 0;
    virtual void layout() = 0;
};

#endif // PAGES_H_INCLUDED
