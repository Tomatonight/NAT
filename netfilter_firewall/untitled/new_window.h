#ifndef NEW_WINDOW_H
#define NEW_WINDOW_H

#include <QFrame>
#include<QTextEdit>
namespace Ui {
class new_window;
}

class new_window : public QFrame
{
    Q_OBJECT

public:
    explicit new_window(QWidget *parent = nullptr);
    ~new_window();
    bool judge_leagal_values(char* s_ip,char* s_mask,char* d_ip,char* d_mask);
private:
    QTextEdit *List;
    Ui::new_window *ui;
};

#endif // NEW_WINDOW_H
