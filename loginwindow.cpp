#include "loginwindow.h"
#include "ui_loginwindow.h"

/*
 * Constructor de LoginWindow.
 * Configura la ventana de login: título, tamaño fijo,
 * estilo del mensaje de error y conexión del botón.
 */
LoginWindow::LoginWindow(QWidget *parent) : QDialog(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    setWindowTitle("Iniciar Sesión");
    setFixedSize(350, 250);

    // El label de error se muestra en rojo y vacío hasta que haya un intento fallido
    ui->labelError->setStyleSheet("color: red;");
    ui->labelError->clear();

    // Signal & Slot: al presionar "Ingresar" se ejecuta onLoginClicked
    connect(ui->botonIngresar, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
}

// Destructor: libera la memoria del UI generado por Qt Designer
LoginWindow::~LoginWindow()
{
    delete ui;
}

/*
 * Slot: valida las credenciales ingresadas por el usuario.
 * Si son correctas, instancia el objeto correspondiente (UsuarioComun o Administrador)
 * y llama a accept() para cerrar el diálogo con resultado positivo.
 * Si son incorrectas, muestra un mensaje de error sin cerrar la ventana.
 *
 * Aquí se aplica herencia: según las credenciales se crea una instancia
 * de la clase derivada adecuada, ambas compatibles con el puntero base Usuario*.
 */
void LoginWindow::onLoginClicked()
{
    QString usuario = ui->campoUsuario->text().trimmed();
    QString contrasena = ui->campoContrasena->text();

    if (usuario == "admin" && contrasena == "admin123") {
        // Crea un Administrador: tiene historial, cambio de tema e imágenes ilimitadas
        usuarioLogueado = new Administrador("admin", "admin123");
        accept(); // Cierra el diálogo con QDialog::Accepted
    } else if (usuario == "user" && contrasena == "user123") {
        // Crea un UsuarioComun: limitado a 5 imágenes por sesión
        usuarioLogueado = new UsuarioComun("user", "user123");
        accept();
    } else {
        // Credenciales incorrectas: muestra error sin cerrar la ventana
        ui->labelError->setText("Usuario o contraseña incorrectos.");
    }
}