#ifndef GY801_H
#define GY801_H

# include <QMainWindow>
# include <QLineEdit>
# include <Qt3DCore/qentity.h>
# include <Qt3DCore/qtransform.h>
# include <Qt3DExtras/QPhongMaterial>
# include <Qt3DExtras/QCylinderMesh>
# include <Qt3DExtras/Qt3DWindow>
# include <Qt3DExtras/QForwardRenderer>
# include <Qt3DRender/QCamera>
# include <Qt3DRender/QPointLight>
# include <QSerialPort>
# include <QDialog>
# include <cmath>
# include "console.h"

QT_BEGIN_NAMESPACE
namespace Ui { class gy801; }
QT_END_NAMESPACE

class gy801 : public QMainWindow
{
    Q_OBJECT

public:
   Qt3DCore::QEntity *init_root_entity(void);
   void add_cylinder(Qt3DCore::QEntity *rootEntity);
   void init_COM(void);
   void update_render(QString data);
   void init_console(void);
   void init_dialog(void);
    gy801(void);
    ~gy801(void);

private slots:
    void on_button_start_clicked(void);
    void on_button_packet_clicked(void);
    void on_button_dialog_apply_clicked(void);
    void read_serial(void);

private:
    Ui::gy801            *ui;
    Qt3DCore::QTransform *cylinderTransform;
    QSerialPort          *COM_port;
    Console              *console;
    QLineEdit            dialog_line_edits[5];
    QDialog              dialog;
    QString              packet_separator = ";";
    int                  packet_size = 17;
    int                  packet_ax_index = 3;
    int                  packet_ay_index = 4;
    int                  packet_az_index = 5;
};

#endif // GY801_H
