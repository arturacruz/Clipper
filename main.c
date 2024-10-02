#include <stdio.h>
#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *scrollWindow;

    GtkWidget *box;
    GtkWidget *button;

    GtkWidget *textView;
    GtkTextBuffer *textBuffer;
} GlobalWindow;

static void loadFile(GObject *source, GAsyncResult *result, GlobalWindow *globalWindow)
{
    g_printerr("Converting pointer.\n");

    GFile *file = G_FILE(source);
    g_autofree char *contents = NULL;
    gsize lenght = 0;

    g_autoptr (GError) error = NULL;

    g_file_load_contents_finish(file, result, &contents, &lenght, NULL, &error);

    if(error != NULL)
    {
        g_printerr("Unable to open file at %s. ERROR: %s\n", g_file_peek_path(file), error->message);
        return;
    }

    g_printerr("Setting new buffer text.\n");

    
    if (globalWindow == NULL) 
    {
        g_printerr("NULL.\n");
    }   
    gtk_text_buffer_set_text(globalWindow->textBuffer, contents, lenght);
    g_printerr("Sucessful.\n");
    GtkTextIter *cursor;
    gtk_text_buffer_get_start_iter(globalWindow->textBuffer, cursor);
    gtk_text_buffer_place_cursor(globalWindow->textBuffer, cursor);
}

static void onSelectFile(GObject *source, GAsyncResult *result, gpointer globalWindow)
{
    GlobalWindow *window = globalWindow;
    GtkFileDialog *dialog = GTK_FILE_DIALOG (source);
    g_autoptr (GFile) file = gtk_file_dialog_open_finish(dialog, result, NULL);

    if(file == NULL)
    {
        g_printerr("File is NULL.\n");
        return;
    }

    g_printerr("Selected file succesfully. Loading file contents.\n");
    g_file_load_contents_async(file, NULL, (GAsyncReadyCallback) loadFile, window);
}

static void onButtonClick(GtkButton *button, GtkWindow *window, gpointer globalWindow)
{
    const char *buttonLabel = gtk_button_get_label(button);

    // If didn't click the button yet
    if(g_strcmp0(buttonLabel, "Open") == 0)
    {
        // Set label to "Opening..."
        gtk_button_set_label(button, "Opening...");

        // Creates a new file dialog
        GtkFileDialog *fileDialog = gtk_file_dialog_new();

        g_printerr("Created file dialog.\n");
        // TODO: actually input some files
        gtk_file_dialog_open(fileDialog, GTK_WINDOW(window), NULL, onSelectFile, globalWindow);
    }
    else // If label is "Opening..."
    {
         gtk_button_set_label(button, "Open");
    }  
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GlobalWindow *globalWindow = malloc(sizeof(GlobalWindow));

    

    GtkWidget *window = globalWindow->window;
    GtkWidget *scrollWindow = globalWindow->scrollWindow;

    GtkWidget *box = globalWindow->box;
    GtkWidget *button = globalWindow->button;

    GtkWidget *textView = globalWindow->textView;
    GtkTextBuffer *textBuffer = globalWindow->textBuffer;

    gchar *defaultText = "Ipsum louco latim doido";

    // Creates a new window
    // Set size and title
    window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Editor Foda");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    // Creates box and sets it as child of window
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
    gtk_window_set_child(GTK_WINDOW(window), box);


    // Creates button and adds it to box
    button = gtk_button_new_with_label("Open");
    g_signal_connect(button, "clicked", G_CALLBACK(onButtonClick), globalWindow);
    gtk_box_append(GTK_BOX(box), button);

    // Creates a scrollable window and adds it to box
    scrollWindow = gtk_scrolled_window_new();
    gtk_box_append(GTK_BOX(box), scrollWindow);

    // Creates text view and buffer for text editing
    textView = gtk_text_view_new();
    textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
    gtk_text_buffer_set_text(textBuffer, defaultText, -1);

    // Remove all line wrapping
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_NONE);

    // Sets textView as child of scrolled window
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollWindow), textView);

    // Shows window
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    // Creates the application
    GtkApplication *app;
    app = gtk_application_new("com.github.arturacruz.editor", G_APPLICATION_DEFAULT_FLAGS);

    // Connects the activate signal to the activate function
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Current application status
    int status;
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Free app
    g_object_unref(app);
    return status;
}

