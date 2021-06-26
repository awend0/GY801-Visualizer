#include "gy801.h"
#include "ui_gy801.h"

/*
 * Initialization
*/

gy801::gy801(void)
{
    COM_port = new QSerialPort(this);

    ui = new Ui::gy801;
    ui->setupUi(this);
    add_cylinder(init_root_entity());
    init_console();
    init_dialog();
    console->putData("Init OK!\n");
}

void gy801::init_dialog(void)
{
    static QLabel       dialog_hints[5];
    static QGridLayout  dialog_layout;
    static QPushButton  dialog_apply_button;

    dialog_apply_button.setText("APPLY");
    connect(&dialog_apply_button, &QPushButton::clicked, this, &gy801::on_button_dialog_apply_clicked);
    dialog_hints[0].setText("Packet separator: ");
    dialog_line_edits[0].setText(";");
    dialog_hints[1].setText("Packet normal size: ");
    dialog_line_edits[1].setText("17");
    dialog_hints[2].setText("Packet AX value index: ");
    dialog_line_edits[2].setText("3");
    dialog_hints[3].setText("Packet AY value index: ");
    dialog_line_edits[3].setText("4");
    dialog_hints[4].setText("Packet AZ value index: ");
    dialog_line_edits[4].setText("5");

    for (int i = 0; i < 5; i++)
    {
        dialog_layout.addWidget(&dialog_hints[i], i, 0);
        dialog_layout.addWidget(&dialog_line_edits[i], i, 1);
    }
    dialog_layout.addWidget(&dialog_apply_button, 5, 0);

    dialog.setLayout(&dialog_layout);
}

Qt3DCore::QEntity *gy801::init_root_entity(void)
{
    Qt3DExtras::Qt3DWindow  *view = new Qt3DExtras::Qt3DWindow();
    Qt3DCore::QEntity       *rootEntity = new Qt3DCore::QEntity();
    Qt3DRender::QCamera     *cameraEntity = view->camera();
    Qt3DCore::QEntity       *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    Qt3DCore::QTransform    *lightTransform = new Qt3DCore::QTransform(lightEntity);
    QWidget                 *render_widget = QWidget::createWindowContainer(view);

    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    cameraEntity->setPosition(QVector3D(0, 0, 0));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);
    view->setRootEntity(rootEntity);
    ui->render_layout->addWidget(render_widget);
    return (rootEntity);
}

void gy801::add_cylinder(Qt3DCore::QEntity *rootEntity)
{
    Qt3DCore::QEntity           *cylinderEntity;
    Qt3DExtras::QCylinderMesh   *cylinder = new Qt3DExtras::QCylinderMesh();
    Qt3DExtras::QPhongMaterial  *cylinderMaterial = new Qt3DExtras::QPhongMaterial();

    cylinder->setRadius(1);
    cylinder->setLength(3);
    cylinder->setRings(100);
    cylinder->setSlices(20);

    cylinderTransform = new Qt3DCore::QTransform();
    cylinderTransform->setScale(1.5f);
    cylinderTransform->setRotationX(0);
    cylinderTransform->setRotationY(0);
    cylinderTransform->setRotationZ(0);
    cylinderTransform->setTranslation(QVector3D(0, 0, -20.0f));

    cylinderMaterial->setDiffuse(QColor(QRgb(0x928327)));

    cylinderEntity = new Qt3DCore::QEntity(rootEntity);
    cylinderEntity->addComponent(cylinder);
    cylinderEntity->addComponent(cylinderMaterial);
    cylinderEntity->addComponent(cylinderTransform);
    cylinderEntity->setEnabled(1);
}

void gy801::init_COM(void)
{
    COM_port->setPortName(ui->line_edit_COM->text());
    COM_port->setBaudRate(ui->line_edit_baud->text().toInt());
    COM_port->setDataBits(QSerialPort::Data8);
    COM_port->setFlowControl(QSerialPort::NoFlowControl);
    COM_port->setParity(QSerialPort::NoParity);
    COM_port->setStopBits(QSerialPort::TwoStop);
    COM_port->open(QSerialPort::ReadOnly);
    QObject::connect(COM_port, SIGNAL(readyRead()), this, SLOT(read_serial()));
}

void gy801::init_console(void)
{
    console = new Console;
    console->setEnabled(1);
    console->setLocalEchoEnabled(1);
    ui->console_layout->addWidget(console);
}

gy801::~gy801(void)
{
    delete COM_port;
    delete cylinderTransform;
    delete console;
    delete ui;
}

/*
 * Slots handlers
*/

void gy801::read_serial(void)
{
    QString dataRead = COM_port->readAll();
    QStringList dataSplitted = dataRead.split('\n');
    for (auto& item : dataSplitted)
        if (item.length() ^ 0)
            update_render(item);
}

void gy801::on_button_dialog_apply_clicked(void)
{
    packet_separator = dialog_line_edits[0].text();
    packet_size = dialog_line_edits[1].text().toInt();
    packet_ax_index = dialog_line_edits[2].text().toInt();
    packet_ay_index = dialog_line_edits[3].text().toInt();
    packet_az_index = dialog_line_edits[4].text().toInt();
    dialog.hide();
}

void gy801::on_button_packet_clicked(void)
{
    dialog.exec();
}

void gy801::on_button_start_clicked(void)
{
    console->putData("Connecting!\n");
    init_COM();
    if (!COM_port->isOpen())
    {
        console->putData("Failed to open port!\n");
        return;
    }
    console->putData("Connected!\n");
}

/*
 * Spinning cylinder
*/

void gy801::update_render(QString data)
{
    QStringList values = data.split(packet_separator);
    float ax, ay, az;
    float pitch, yaw, roll;
    if (values.size() != packet_size)
    {
        console->putData("Packet corrupted! Size: " + QByteArray::number(values.size()) + "\n");
        return;
    }
    ax = values[packet_ax_index].toFloat();
    ay = values[packet_ay_index].toFloat();
    az = values[packet_az_index].toFloat();
    pitch = 180.0f * atan(ax / sqrt(ay * ay + az * az)) / M_PI;
    roll = 180.0f * atan(ay / sqrt(ax * ax + az * az)) / M_PI;
    yaw = 180.0f * atan(az / sqrt(ax * ax + az * az)) / M_PI;
    console->putData("Packet OK! PYR: " + QByteArray::number(pitch)
        + " " + QByteArray::number(yaw) + " " + QByteArray::number(roll) + "\n");
    cylinderTransform->setRotation(QQuaternion::fromEulerAngles(pitch, yaw, roll));
}
