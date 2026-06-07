#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include "usuario.h" // Jerarquía de clases de usuario

QT_BEGIN_NAMESPACE
namespace Ui { class LoginWindow; }
QT_END_NAMESPACE

/*
 * Clase LoginWindow: ventana de autenticación de la aplicación.
 * Hereda de QDialog (ventana secundaria modal de Qt).
 * Es la primera ventana que ve el usuario al iniciar la app.
 * Según las credenciales ingresadas, crea un UsuarioComun o un Administrador.
 */
class LoginWindow : public QDialog {
    Q_OBJECT // Macro de Qt necesaria para usar signals & slots

public:
    // Constructor: inicializa la ventana de login
    LoginWindow(QWidget *parent = nullptr);

    // Destructor: libera la memoria del UI generado por Qt Designer
    ~LoginWindow();

    // Getter: retorna el usuario autenticado tras un login exitoso.
    // Retorna nullptr si el login no fue completado.
    Usuario* getUsuarioLogueado() const { return usuarioLogueado; }

private slots:
    // Slot: se ejecuta al presionar el botón "Ingresar".
    // Valida las credenciales y crea el objeto de usuario correspondiente.
    void onLoginClicked();

private:
    Ui::LoginWindow *ui;                  // UI generado desde loginwindow.ui
    Usuario *usuarioLogueado = nullptr;   // Puntero al usuario autenticado (null hasta login exitoso)
};

#endif // LOGINWINDOW_H