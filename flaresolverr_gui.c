#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
    GtkWidget *window;
} AppState;

void update_title(AppState *state, const char *title) {
    if (state && state->window) {
        gtk_window_set_title(GTK_WINDOW(state->window), title);
    }
}

void on_start_clicked(GtkWidget *widget, gpointer data) {
    AppState *state = (AppState *)data;

    if (fork() == 0) {
        // Child process
        execlp("gnome-terminal", "gnome-terminal", "--title=FlareSolverr", "--", "/home/delta/src/flaresolverr/run", (char *)NULL);
        exit(1);
    }

    update_title(state, "FlareSolverr - Running");
}

void on_stop_clicked(GtkWidget *widget, gpointer data) {
    AppState *state = (AppState *)data;

    FILE *fp = fopen("/tmp/flaresolverr_python.pid", "r");
    if (fp) {
        int pid;
        if (fscanf(fp, "%d", &pid) == 1) {
            kill(pid, SIGKILL); // Using SIGKILL for guaranteed stop
            printf("Killed process %d\n", pid);
        }
        fclose(fp);
    } else {
        printf("PID file not found. Is it running?\n");
    }

    remove("/tmp/flaresolverr_python.pid");
    update_title(state, "FlareSolverr - Stopped");
}

int main(int argc, char *argv[]) {
    // Fix 1: Allocate state on the heap so the pointer is always valid
    AppState *state = malloc(sizeof(AppState)); 
    
    GtkWidget *window;
    GtkWidget *main_hbox;
    GtkWidget *logo;
    GtkWidget *btn_start;
    GtkWidget *btn_stop;
    GtkWidget *btn_box;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    state->window = window;
    
    gtk_window_set_title(GTK_WINDOW(window), "FlareSolverr Controller");
    gtk_window_set_keep_above(GTK_WINDOW(window), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    gtk_container_add(GTK_CONTAINER(window), main_hbox);

    // Logo
    logo = gtk_image_new_from_file("/home/delta/src/flaresolverr/FlareSolverr/resources/flaresolverr_logo.png");
    gtk_box_pack_start(GTK_BOX(main_hbox), logo, FALSE, FALSE, 0);

    // Button Column
    btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(main_hbox), btn_box, TRUE, TRUE, 0);

    btn_start = gtk_button_new_with_label("Start Script");
    g_signal_connect(btn_start, "clicked", G_CALLBACK(on_start_clicked), state);
    gtk_box_pack_start(GTK_BOX(btn_box), btn_start, TRUE, TRUE, 0);

    btn_stop = gtk_button_new_with_label("Stop Script");
    g_signal_connect(btn_stop, "clicked", G_CALLBACK(on_stop_clicked), state);
    gtk_box_pack_start(GTK_BOX(btn_box), btn_stop, TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    free(state); // Clean up memory
    return 0;
}
