#ifndef INFOUSUARIOWINDOW_H
#define INFOUSUARIOWINDOW_H

#include <QDialog>
#include "usuario.h" // Necesario para acceder a los datos del usuario

QT_BEGIN_NAMESPACE
namespace Ui { class InfoUsuarioWindow; }
QT_END_NAMESPACE

/*
 * Clase InfoUsuarioWindow: ventana secundaria de información del usuario.
 * Hereda de QDialog y se abre de forma modal desde MainWindow.
 * Muestra el nombre, tipo de cuenta e imágenes generadas en la sesión.
 * Al recibir un puntero a Usuario, funciona tanto para UsuarioComun
 * como para Administrador gracias al polimorfismo.
 */
class InfoUsuarioWindow : public QDialog
{
    Q_OBJECT // Macro de Qt necesaria para usar signals & slots

public:
    // Constructor: recibe el usuario actual para mostrar sus datos
    InfoUsuarioWindow(Usuario *usuario, QWidget *parent = nullptr);

    // Destructor: libera la memoria del UI generado por Qt Designer
    ~InfoUsuarioWindow();

private:
    Ui::InfoUsuarioWindow *ui; // UI generado desde infousuariowindow.ui
};

#endif // INFOUSUARIOWINDOW_H