#include <stdio.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct {
    GtkWindow *window;

    GtkWidget *headerBar;
    GtkWidget *menuButton;
    GtkWidget *mainMenu;

    GtkWidget *notebook;

} GlobalWindow;

static void newNotebookPage(GlobalWindow *globalWindow)
{
    GtkWidget *scrollWindow = gtk_scrolled_window_new();
    GtkWidget *textView = gtk_text_view_new();
    GtkTextBuffer *textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));

    const char *text = "";
    gtk_text_buffer_set_text(textBuffer, text, 0);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_NONE);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollWindow), textView);
    GtkWidget *titleLabel = gtk_label_new("New file");

    gtk_notebook_append_page(GTK_NOTEBOOK(globalWindow->notebook), scrollWindow, titleLabel);
    GtkNotebookPage *notebookPage = gtk_notebook_get_page(GTK_NOTEBOOK(globalWindow->notebook), scrollWindow);
    g_object_set(notebookPage, "tab-expand", TRUE, NULL);
}

static void loadFile(GObject *source, GAsyncResult *result, GlobalWindow *globalWindow)
{
    GFile *file = G_FILE(source);
    g_autofree char *contents = NULL;
    gsize length = 0;

    g_autoptr (GError) error = NULL;

    g_file_load_contents_finish(file, result, &contents, &length, NULL, &error);

    if(error != NULL)
    {
        g_printerr("Unable to open file at %s: %s\n", g_file_peek_path(file), error->message);
        return;
    }
    if (!g_utf8_validate (contents, length, NULL))
    {
        g_printerr ("Unable to load the contents of “%s”: File not UTF-8 type.\n", g_file_peek_path(file));
        return;
    }

    g_autofree char *displayName = g_file_get_basename(file);

    GtkWidget *scrollWindow = gtk_scrolled_window_new();

    GtkWidget *textView = gtk_text_view_new();
    GtkTextBuffer *textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));

    gtk_text_buffer_set_text(textBuffer, contents, length);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_NONE);

    // Sets textView as child of scrolled window
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollWindow), textView);

    GtkWidget *titleLabel = gtk_label_new(displayName);

    gtk_notebook_append_page(GTK_NOTEBOOK(globalWindow->notebook), scrollWindow, titleLabel);
    GtkNotebookPage *notebookPage = gtk_notebook_get_page(GTK_NOTEBOOK(globalWindow->notebook), scrollWindow);
    g_object_set(notebookPage, "tab-expand", TRUE, NULL);

    g_printerr("Sucessful load.\n");
}

static void saveFile(GObject *source, GAsyncResult *result, GlobalWindow *globalWindow)
{
    g_printerr("Saving file...\n");
    GFile *file = G_FILE(source);
    g_autoptr (GError) error = NULL;

    g_file_replace_contents_finish(file, result, NULL, &error);

    if(error != NULL)
    {
        g_printerr("Unable to save file at %s: %s\n", g_file_peek_path(file), error->message);
        return;
    }

    g_printerr("Sucessful save.\n");
}

static void onSelectFileLoad(GObject *source, GAsyncResult *result, gpointer globalWindow)
{
    GlobalWindow *window = globalWindow;
    GtkFileDialog *dialog = GTK_FILE_DIALOG (source);
    g_autoptr (GFile) file = gtk_file_dialog_open_finish(dialog, result, NULL);

    if(file == NULL)
    {
        g_printerr("File is NULL.\n");
        return;
    }

    g_printerr("Selected file succesfully. Loading contents.\n");
    g_file_load_contents_async(file, NULL, (GAsyncReadyCallback) loadFile, window);
}

static void onSelectFileSave(GObject *source, GAsyncResult *result, gpointer globalWindow)
{
    GlobalWindow *window = globalWindow;
    GtkFileDialog *dialog = GTK_FILE_DIALOG (source);
    g_autoptr (GFile) file = gtk_file_dialog_save_finish(dialog, result, NULL);

    if(file == NULL)
    {
        g_printerr("File is NULL.\n");
        return;
    }

    g_printerr("Selected file succesfully. Saving contents.\n");
    int currentPageIndex = gtk_notebook_get_current_page(GTK_NOTEBOOK(window->notebook));
    GtkWidget *scrollWindow = gtk_notebook_get_nth_page(GTK_NOTEBOOK(window->notebook), currentPageIndex);

    GtkWidget *textView = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(scrollWindow));
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));

    GtkTextIter start;
    gtk_text_buffer_get_start_iter (buffer, &start);

    // Retrieve the iterator at the end of the buffer
    GtkTextIter end;
    gtk_text_buffer_get_end_iter (buffer, &end);

    char *content = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);

    if(content == NULL)
    {
        g_printerr("No text in file to save.\n");
        return;
    }

    g_autoptr (GBytes) bytes = g_bytes_new_take(content, strlen(content));

    g_file_replace_contents_bytes_async(file, bytes, NULL, FALSE, G_FILE_CREATE_NONE, NULL, (GAsyncReadyCallback) saveFile, window);
}

static void onButtonLoadClick(GSimpleAction *action, GVariant *state, gpointer globalWindow)
{
    g_printerr("Clicked LOAD button.\n");

    GlobalWindow *window = globalWindow;

    // Creates a new file dialog
    GtkFileDialog *fileDialog = gtk_file_dialog_new();
    g_printerr("Created file dialog.\n");

    // TODO: actually input some files
    gtk_file_dialog_open(fileDialog, window->window, NULL, onSelectFileLoad, globalWindow);
    
}

static void onButtonNewClick(GSimpleAction *action, GVariant *state, gpointer globalWindow)
{
    GlobalWindow *window = globalWindow;
    newNotebookPage(window);
    g_print("Created new page.\n");
}

static void onButtonSaveClick(GSimpleAction *action, GVariant *state, gpointer globalWindow)
{
    GlobalWindow *window = globalWindow;
    GtkNotebook *notebook = GTK_NOTEBOOK(window->notebook);

    if(gtk_notebook_get_current_page(notebook) == -1)
    {
        g_printerr("No pages currently open to save.\n");
        return;
    }

    GtkFileDialog *fileDialog = gtk_file_dialog_new();
    gtk_file_dialog_save(fileDialog, GTK_WINDOW(window->window), NULL, onSelectFileSave, globalWindow);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GlobalWindow *globalWindow = malloc(sizeof(GlobalWindow));
    char *username = malloc(50 * sizeof(char));
    username = getlogin();

    char path[100] = "/home/";
    char dir[] = "/.local/share/clipper/ui/clipper.ui";

    strcat(path, username);
    strcat(path, dir);

    GtkBuilder *builder = gtk_builder_new_from_file(path);

    // Creates a new window
    // Set size and title
    globalWindow->window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    gtk_window_set_application (globalWindow->window, app);

    globalWindow->notebook = GTK_WIDGET(gtk_builder_get_object(builder, "notebook"));

    globalWindow->mainMenu = GTK_WIDGET(gtk_builder_get_object(builder, "mainMenu"));
    const GActionEntry appEntries[] = {
        {"new", onButtonNewClick, NULL, NULL, NULL},
        {"open", onButtonLoadClick, NULL, NULL, NULL},
        {"saveas", onButtonSaveClick, NULL, NULL, NULL}
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), appEntries, G_N_ELEMENTS(appEntries), globalWindow);

    

    // Shows window
    gtk_window_present(globalWindow->window);

    g_object_unref(builder);
}

int main(int argc, char **argv)
{

    // Creates the application
    GtkApplication *app;
    app = gtk_application_new("com.github.arturacruz.editor", G_APPLICATION_DEFAULT_FLAGS);

    g_printerr("Activating app.\n");
    g_printerr("Initializing application.\n");
    // Connects the activate signal to the activate function
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Current application status
    int status;
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Free app
    g_object_unref(app);
    return status;
}

