#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QHBoxLayout>
#include "usuario.h"           // Jerarquía de clases de usuario
#include "infousuariowindow.h" // Ventana secundaria de información

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*
 * Clase MainWindow: ventana principal de la aplicación.
 * Hereda de QMainWindow (clase de Qt para ventanas principales).
 * Gestiona la generación de imágenes con IA, el historial,
 * y adapta la interfaz según el tipo de usuario logueado.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT // Macro de Qt necesaria para usar signals & slots

public:
    // Constructor: recibe el usuario autenticado para personalizar la interfaz
    MainWindow(Usuario *usuario, QWidget *parent = nullptr);

    // Destructor: libera la memoria del UI generado por Qt Designer
    ~MainWindow();

private slots:
    /*
     * Slots: funciones conectadas a señales de los widgets (mecanismo de Qt).
     * Se ejecutan automáticamente cuando ocurre el evento correspondiente.
     */

    // Se ejecuta al presionar el botón "Generar" — valida y lanza la request a la API
    void onGenerarClicked();

    // Se ejecuta cuando la API responde con la imagen descargada
    void onImagenDescargada(QNetworkReply *reply);

    // Se ejecuta al presionar "Guardar" — abre diálogo para guardar la imagen
    void onGuardarClicked();

    // Se ejecuta al presionar "Borrar Historial" — solo disponible para admin
    void onBorrarHistorialClicked();

    // Se ejecuta al presionar "Información de Usuario" — abre ventana secundaria
    void onInfoUsuarioClicked();

    // Se ejecuta al presionar el botón de tema — alterna entre oscuro y claro
    void onToggleTemaClicked();

private:
    Ui::MainWindow *ui;              // Puntero al UI generado desde el archivo .ui
    QNetworkAccessManager *manager;  // Gestor de peticiones HTTP para la API de imágenes
    QPixmap pixmapActual;            // Almacena la última imagen generada
    QWidget *widgetHistorial;        // Widget contenedor del historial de miniaturas
    QHBoxLayout *layoutHistorial;    // Layout horizontal para organizar las miniaturas
    Usuario *usuarioActual;          // Puntero al usuario autenticado (polimorfismo)
    bool temaOscuro;                 // Estado actual del tema: true = oscuro, false = claro

    // Construye el prompt combinando la descripción del usuario con el modificador de estilo
    QString construirPrompt(const QString &descripcion, const QString &estilo);

    // Agrega una miniatura de la imagen generada al historial (solo admin)
    void agregarAlHistorial(const QPixmap &pixmap);

    // Detecta si el texto ingresado contiene palabras en español
    bool contieneEspanol(const QString &texto);

    // Aplica el stylesheet correspondiente al tema actual en toda la aplicación
    void aplicarTema();
};

#endif // MAINWINDOW_H