#include "infousuariowindow.h"
#include "ui_infousuariowindow.h"
#include <QPushButton>

/*
 * Constructor de InfoUsuarioWindow.
 * Recibe un puntero a Usuario (puede ser UsuarioComun o Administrador).
 * Gracias al polimorfismo, getLimite() y getImagenesGeneradas() retornan
 * los valores correctos según el tipo de usuario sin necesidad de casteos.
 */
InfoUsuarioWindow::InfoUsuarioWindow(Usuario *usuario, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InfoUsuarioWindow)
{
    ui->setupUi(this);
    setWindowTitle("Información de usuario");
    setFixedSize(300, 200);

    // Muestra el nombre y tipo de cuenta usando métodos virtuales de Usuario
    ui->labelNombre->setText("Nombre: " + usuario->getNombre());
    ui->labelTipo->setText("Tipo de cuenta: " + usuario->getTipo());

    // Construye el texto del contador según si el usuario tiene límite o no
    // getLimite() retorna -1 para Administrador y 5 para UsuarioComun
    QString infoImagenes;
    if (usuario->getLimite() == -1) {
        infoImagenes = QString("Imágenes generadas: %1 (ilimitado)")
                           .arg(usuario->getImagenesGeneradas());
    } else {
        infoImagenes = QString("Imágenes generadas: %1 / %2")
                           .arg(usuario->getImagenesGeneradas())
                           .arg(usuario->getLimite());
    }
    ui->labelImagenes->setText(infoImagenes);

    // Signal & Slot: el botón cerrar cierra el diálogo
    connect(ui->botonCerrar, &QPushButton::clicked, this, &QDialog::close);
}

// Destructor: libera la memoria del UI generado por Qt Designer
InfoUsuarioWindow::~InfoUsuarioWindow()
{
    delete ui;
}