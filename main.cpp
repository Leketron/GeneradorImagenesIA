#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>
#include <QMessageBox>

/*
 * Punto de entrada de la aplicación.
 * Sigue el flujo: Login → (si es exitoso) → Ventana Principal.
 * Utiliza polimorfismo: getUsuarioLogueado() retorna un Usuario*
 * que puede ser UsuarioComun o Administrador según las credenciales.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Evita que la app se cierre al cerrar el diálogo de login
    // (por defecto Qt cierra la app cuando se cierra la última ventana)
    a.setQuitOnLastWindowClosed(false);

    // Muestra la ventana de login de forma modal y espera la respuesta
    LoginWindow login;
    if (login.exec() != QDialog::Accepted) {
        // El usuario cerró el login sin ingresar — se termina la aplicación
        return 0;
    }

    // Obtiene el usuario autenticado (UsuarioComun o Administrador)
    // El puntero base Usuario* permite trabajar con ambos tipos sin distinción
    Usuario *usuario = login.getUsuarioLogueado();

    // Crea y muestra la ventana principal maximizada
    // MainWindow recibe el usuario y adapta la interfaz según su tipo
    MainWindow *w = new MainWindow(usuario);
    w->showMaximized();

    // Inicia el loop de eventos de Qt — la app corre hasta que se cierre MainWindow
    return a.exec();
}