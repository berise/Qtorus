#include "about.h"
#include <QLayout>


// ----------------------------------------------------------------------------
// AboutDialog
// ----------------------------------------------------------------------------
AboutDialog::AboutDialog(QWidget *parent)
             : QDialog(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(new QPushButton("&Ok"));
    setLayout(layout);

    setWindowTitle(tr("About Torus"));
}
