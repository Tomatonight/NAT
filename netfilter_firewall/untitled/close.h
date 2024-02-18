#ifndef CLOSE_H
#define CLOSE_H

#include <QFrame>

namespace Ui {
class Close;
}

class Close : public QFrame
{
    Q_OBJECT

public:
    explicit Close(QWidget *parent = nullptr);
    ~Close();

private:
    Ui::Close *ui;
};

#endif // CLOSE_H
