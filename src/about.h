#include <QDialog>
#include <QPushButton>

// A custom modal dialog
class AboutDialog : public QDialog
{
 Q_OBJECT
public:
    AboutDialog(QWidget *parent = NULL);   

private:
	QPushButton *m_btnOK;
	QPushButton *m_btnCancel;
};

