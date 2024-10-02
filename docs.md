# Learning GTK

### Creating a window

```c
GtkWidget *window;

window = gtk_application_window_new(GTK_APPLICATION(app));
gtk_window_set_title(GTK_WINDOW(window), "Editor Foda");
gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
```

### Creating a label

```c
label = gtk_label_new("Hello.");
```

### Creating a button
```c
GtkWidget *button;

button = gtk_button_new_with_label("Aperta");
g_signal_connect(button, "clicked", G_CALLBACK(onButtonClick), NULL);
```

### Creating a box
```c
GtkWidget *box;

box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
gtk_box_set_homogeneous(GTK_BOX(box), TRUE);
gtk_window_set_child(GTK_WINDOW(window), box);
```



### Adding things to the box

```c
gtk_box_append(GTK_BOX(box), label);
gtk_box_append(GTK_BOX(box), button);
```

### Callback function and comparing labels

```c
static void onButtonClick(GtkButton *button, GtkWindow *window)
{
    const char *buttonLabel = gtk_button_get_label(button);

    if(g_strcmp0(buttonLabel, "Aperta") == 0)
    {
        gtk_button_set_label(button, "Apertou!");
    }
    else
    {
        gtk_window_destroy(window);
    }
}
```
