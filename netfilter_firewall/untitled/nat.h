#ifndef NAT_H
#define NAT_H

#include <QFrame>

namespace Ui {
class nat;
}

class nat : public QFrame
{
    Q_OBJECT

public:
    explicit nat(QWidget *parent = nullptr);
    ~nat();

private:
    Ui::nat *ui;
};

#endif // NAT_H
