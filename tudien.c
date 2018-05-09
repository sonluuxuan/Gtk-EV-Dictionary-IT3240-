#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
//#include "bt-5.0.0/inc/btree.h"
#include <gdk/gdkkeysyms.h>
#include "./inc/btree.h"
#include <stdlib.h>


typedef struct {
    char   word[30];
    BTA * tree;
    int no_match;
    GtkWidget * word_translation;
    int cnt_tab;
    char suggest[5][30];

} data;

typedef struct
{
    BTA *tree;
    GtkWidget *parent_window;   
} modify_struct;

typedef struct 
{
    BTA *tree;
    GtkWidget *add_window;
    GtkWidget *word_entry;
    GtkWidget *translation_entry;
} add_struct;

typedef struct 
{
    BTA *tree;
    GtkWidget *window;
    GtkWidget *word_entry;
} remove_struct;


gboolean func(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data) {
    
    return TRUE;
}

void soundex(name, sound)
  char   * name;
  char   * sound;  /* supply at least 5 pre-allocated characters */
{
  char   * in = name,
         * out = sound;
  char   prev = '\0';
  int    is_first = 1;
  char   code;

  while (*in != '\0' && out - sound < 4)
  {
    switch(tolower(*in))
    {
      case 'b':
      case 'f':
      case 'p':
      case 'v':
        code = '1';
      break;

      case 'c':
      case 'g':
      case 'j':
      case 'k':
      case 'q':
      case 's':
      case 'x':
      case 'z':
        code = '2';
      break;

      case 'd':
      case 't':
        code = '3';
      break;

      case 'l':
        code = '4';
      break;

      case 'm':
      case 'n':
        code = '5';
      break;

      case 'r':
        code = '6';
      break;

      /* includes "aehiouwy", punctuation, numbers and control chars */
      default:
        code = 0;
      break;
    }

    if (is_first)
    {
      *out++ = toupper(*in);
      is_first = 0;
    }
    else if (code && prev != code)
      *out++ = code;
    prev = code;

    in++;
  }

  while (out - sound < 4)
    *out++ = '0';

  *out = '\0';

}



gboolean on_key_press (GtkEntry *entry, GdkEventKey *event, gpointer user_data);

gboolean delete_entry(GtkEntry * entry, GdkEventKey * event, gpointer user_data){
      const gchar * entry_word;
      GtkListStore *ls = gtk_list_store_new(1, G_TYPE_STRING);
      GtkTreeIter iter;
      GtkEntryCompletion *completion = gtk_entry_completion_new();

      entry_word =  gtk_entry_get_text(GTK_ENTRY(entry));
      printf("entry word: %s\n", entry_word);

}

/*typedef struct
{
    BTA *tree;
    GtkWidget *word_translation;
} my_struct;*/


static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit ();
    return FALSE;
}

char *get_text_of_textview(GtkWidget *textView) 
{
    GtkTextIter start, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer((GtkTextView *)textView);
    gchar *text;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_iter_forward_line(&end);
    text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    return text;
}


static void enter_callback (GtkWidget *entry, gpointer user_data)
{
      const gchar *entry_text;
      char word[50];
      entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
      strcpy(word, entry_text);
      char translation[100000];
      int real_size;
      btsel(((data*)user_data)->tree, word, translation, 100000, &real_size);
      GtkTextBuffer *buffer;
      buffer = gtk_text_buffer_new(NULL);
      gtk_text_buffer_set_text(buffer, translation, real_size);
      gtk_text_view_set_buffer(GTK_TEXT_VIEW(((data*)user_data)->word_translation), buffer);
  }

static void add_callback(GtkWidget *button, gpointer data)
{
    GtkWidget *entry;
    GtkWidget *translation_view;
    GtkWidget *window;
    BTA *tree;
    entry = ((add_struct *)data)->word_entry;
    translation_view = ((add_struct *)data)->translation_entry;
    window = ((add_struct *)data)->add_window;
    tree = ((add_struct *)data)->tree;
    // get input from word_entry;
    const gchar *entry_text;
    char word[50];
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    strcpy(word, entry_text);
    printf("%s\n", word);
    // get input from translation_entry;
    char translation[100000];
    strcpy(translation, get_text_of_textview(translation_view));
    printf("%s\n", translation);
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(translation_view));
    printf("%d\n", gtk_text_buffer_get_line_count(buffer)); 

    GtkWidget *dialog;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    if(btins(tree, word, translation, strlen(translation)) == 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Success!");
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    else
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Fail! Please re-check your inputs.");
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    gtk_widget_destroy(window);
}

void on_add_clicked(GtkButton* button, gpointer data)
{
  GtkWidget *add_window;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *translation_label;
  GtkWidget *translation;
  GtkWidget *cf_button;
  GtkWidget *pWindow;
  pWindow = ((modify_struct *)data)->parent_window;
  BTA *tree;
  tree = ((modify_struct *)data)->tree;
  add_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // setting mini window properties //
  gtk_window_set_title(GTK_WINDOW(add_window), "Adding a new word");
  gtk_container_set_border_width(GTK_CONTAINER(add_window), 10);
  gtk_window_set_resizable(GTK_WINDOW(add_window), FALSE);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(add_window), TRUE);
  gtk_window_set_skip_pager_hint(GTK_WINDOW(add_window), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(add_window), GTK_WINDOW(pWindow));
  gtk_window_set_position(GTK_WINDOW(add_window), GTK_WIN_POS_CENTER_ON_PARENT);
  // create a grid //
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  // put the grid into the window //
  gtk_container_add (GTK_CONTAINER(add_window), grid);
  // create and add a label to the grid //
  label = gtk_label_new("Word:  ");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
  // create an entry and add it to the grid //
  entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
  gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 2, 1);
  // create and add a translation_label to the grid //
  translation_label = gtk_label_new("Translation:  ");
  gtk_grid_attach(GTK_GRID(grid), translation_label, 0, 1, 1, 1);
  // create a text view and add it to the grid //
  translation = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(translation), TRUE);
  gtk_widget_set_size_request(translation, 300, 150);
  gtk_grid_attach(GTK_GRID(grid), translation, 0, 2, 5, 1);
  // create a button and add it to the grid //
  cf_button = gtk_button_new_with_label ("Confirm");
  gtk_grid_attach(GTK_GRID(grid), cf_button, 2, 3, 1, 1);
  // add word and translation to btree file when button is clicked //
  add_struct *add_data;
  add_data = malloc(sizeof(add_struct));
  add_data->tree = tree;
  add_data->add_window = add_window;
  add_data->word_entry = entry;
  add_data->translation_entry = translation;
  g_signal_connect(cf_button, "clicked", G_CALLBACK(add_callback), add_data);
  gtk_widget_show_all (add_window);
}

static void remove_callback(GtkWidget *button, gpointer data)
{
    GtkWidget *entry;
    GtkWidget *window;
    BTA *tree;
    entry = ((remove_struct *)data)->word_entry;
    window = ((remove_struct *)data)->window;
    tree = ((remove_struct *)data)->tree;
    // get input from word_entry;
    const gchar *entry_text;
    char word[50];
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    strcpy(word, entry_text);
    printf("%s\n", word);

    GtkWidget *dialog;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    if(btdel(tree, word) == 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Success!");
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    else
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Fail! Please re-check your inputs.");
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    gtk_widget_destroy(window);
}

void on_remove_clicked(GtkButton* button, gpointer data)
{
  GtkWidget *remove_window;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *cf_button;
  GtkWidget *pWindow;
  pWindow = ((modify_struct*)data)->parent_window;
  BTA *tree;
  tree = ((modify_struct *)data)->tree;
  remove_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // setting mini window properties //
  gtk_window_set_title(GTK_WINDOW(remove_window), "Removing a word");
  gtk_container_set_border_width(GTK_CONTAINER(remove_window), 10);
  gtk_window_set_resizable(GTK_WINDOW(remove_window), FALSE);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(remove_window), TRUE);
  gtk_window_set_skip_pager_hint(GTK_WINDOW(remove_window), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(remove_window), GTK_WINDOW(pWindow));
  gtk_window_set_position(GTK_WINDOW(remove_window), GTK_WIN_POS_CENTER_ON_PARENT);
  // create a grid //
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  // put the grid into the window //
  gtk_container_add (GTK_CONTAINER(remove_window), grid);
  // create and add a label to the grid //
  label = gtk_label_new("Word:  ");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
  // create an entry and add it to the grid //
  entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
  gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 2, 1);
  // create a button and add it to the grid //
  cf_button = gtk_button_new_with_label ("Confirm");
  gtk_grid_attach(GTK_GRID(grid), cf_button, 1, 1, 1, 1);
  remove_struct *remove_data;
  remove_data = malloc(sizeof(remove_struct));
  remove_data->tree = tree;
  remove_data->window = remove_window;
  remove_data->word_entry = entry;
  g_signal_connect(cf_button, "clicked", G_CALLBACK(remove_callback), remove_data);
  gtk_widget_show_all (remove_window);
}

static void change_callback(GtkWidget *button, gpointer data)
{
    GtkWidget *entry;
    GtkWidget *translation_view;
    GtkWidget *window;
    BTA *tree;
    entry = ((add_struct *)data)->word_entry;
    translation_view = ((add_struct *)data)->translation_entry;
    window = ((add_struct *)data)->add_window;
    tree = ((add_struct *)data)->tree;
    // get input from word_entry;
    const gchar *entry_text;
    char word[50];
    entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
    strcpy(word, entry_text);
    printf("%s\n", word);
    // get input from translation_entry;
    char translation[100000];
    strcpy(translation, get_text_of_textview(translation_view));
    printf("%s\n", translation);
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(translation_view));
    printf("%d\n", gtk_text_buffer_get_line_count(buffer)); 

    GtkWidget *dialog;
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    if(btupd(tree, word, translation, strlen(translation)) == 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Success!");
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    else
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(window), flags, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Fail! Please re-check your inputs.");
        gtk_dialog_run(GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
    }
    gtk_widget_destroy(window);
}

void on_change_clicked(GtkButton* button, gpointer data)
{
  GtkWidget *change_window;
  GtkWidget *grid;
  GtkWidget *label;
  GtkWidget *entry;
  GtkWidget *translation_label;
  GtkWidget *translation;
  GtkWidget *cf_button;
  GtkWidget *pWindow;
  pWindow = ((modify_struct *)data)->parent_window;
  BTA *tree;
  tree = ((modify_struct *)data)->tree;
  change_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // setting mini window properties //
  gtk_window_set_title(GTK_WINDOW(change_window), "Changing a word's translation");
  gtk_container_set_border_width(GTK_CONTAINER(change_window), 10);
  gtk_window_set_resizable(GTK_WINDOW(change_window), FALSE);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(change_window), TRUE);
  gtk_window_set_skip_pager_hint(GTK_WINDOW(change_window), TRUE);
  gtk_window_set_transient_for(GTK_WINDOW(change_window), GTK_WINDOW(pWindow));
  gtk_window_set_position(GTK_WINDOW(change_window), GTK_WIN_POS_CENTER_ON_PARENT);
  // create a grid //
  grid = gtk_grid_new();
  gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
  gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
  // put the grid into the window //
  gtk_container_add (GTK_CONTAINER(change_window), grid);
  // create and add a label to the grid //
  label = gtk_label_new("Word:  ");
  gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
  // create an entry and add it to the grid //
  entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
  gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 2, 1);
  // create and add a translation_label to the grid //
  translation_label = gtk_label_new("Translation:  ");
  gtk_grid_attach(GTK_GRID(grid), translation_label, 0, 1, 1, 1);
  // create a text view and add it to the grid //
  translation = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(translation), TRUE);
  gtk_widget_set_size_request(translation, 300, 150);
  gtk_grid_attach(GTK_GRID(grid), translation, 0, 2, 5, 1);
  // create a button and add it to the grid //
  cf_button = gtk_button_new_with_label ("Confirm");
  gtk_grid_attach(GTK_GRID(grid), cf_button, 2, 3, 1, 1);
  // add word and translation to btree file when button is clicked //
  add_struct *change_data;
  change_data = malloc(sizeof(add_struct));
  change_data->tree = tree;
  change_data->add_window = change_window;
  change_data->word_entry = entry;
  change_data->translation_entry = translation;
  g_signal_connect(cf_button, "clicked", G_CALLBACK(change_callback), change_data);
  gtk_widget_show_all (change_window);
}

int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *translation;
    GtkWidget *label;
    GtkWidget *scrolledwindow;
    GtkWidget *button;
    GtkWidget *remove_button;
    GtkWidget *add_button;
    GtkWidget *change_button;
    BTA *tree;
    btinit();
    tree = btopn("database", 0, 0);
    gtk_init (&argc, &argv);
    // create a window with fixed size //
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Anh - Viet Dictionary");
    gtk_window_set_resizable (GTK_WINDOW(window), FALSE);
    gtk_container_set_border_width (GTK_CONTAINER (window), 10);
    // destroy event //
    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    // create a grid for layout //
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    // put the grid into the window //
    gtk_container_add (GTK_CONTAINER(window), grid);
    // create and add a label to the grid //
    label = gtk_label_new("Enter your word:    ");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    // create an entry and add it to the grid //
    entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 2, 1);
    // create a scrolled window and add it to the the grid //
    scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
    //gtk_grid_attach(GTK_GRID(grid), scrolledwindow, 0, 1, 8, 1);
    gtk_widget_set_size_request(scrolledwindow, 700, 600);
    // create an translation entry and add it to the scrolledwindow //
    translation = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(translation), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolledwindow), translation);
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_container_add(GTK_CONTAINER(frame), scrolledwindow);
    gtk_grid_attach(GTK_GRID(grid), frame, 0, 1, 8, 1);
    data *in_data = (data*)malloc(sizeof(data));

    in_data->tree=tree;

    strcpy(in_data->word, "");
    in_data->no_match = 0;
    in_data->cnt_tab = 0;
    for(int k =0; k<5; k++){
      strcpy(in_data->suggest[k], "");
    }
    in_data->word_translation =  translation;

    // declare a data structure to pass all the information required to callback function //
    /*my_struct *data;
    data = malloc(sizeof(my_struct));
    data->word_translation =  translation;
    data->tree = tree;*/
    // get text from entry when user press enter //

    //func used when user delete a word, when the next key pressed, the current text in the entry will be taken, not the text before delete
    g_signal_connect(entry, "delete-text", G_CALLBACK(delete_entry), NULL);

    g_signal_connect(entry, "key_press_event", G_CALLBACK(on_key_press), in_data);
    g_signal_connect(entry, "activate", G_CALLBACK(enter_callback), in_data);
    // create a button and add it to the grid //
    button = gtk_button_new();
    gtk_grid_attach(GTK_GRID(grid), button, 3, 2, 2, 1);

    GtkWidget *quit_image = gtk_image_new_from_file("quit.png");
    gtk_button_set_image(GTK_BUTTON(button), quit_image);
    gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE);
    gtk_button_set_image_position(GTK_BUTTON(button), GTK_POS_BOTTOM);

    // exit program when user press "quit" button //
    g_signal_connect(button, "clicked", G_CALLBACK(delete_event), NULL);

    modify_struct *passing_data;
    passing_data = malloc(sizeof(modify_struct));
    passing_data->parent_window = window;
    passing_data->tree = tree;

    GList *children;

    // create a button and add it to the grid //
    add_button = gtk_button_new_with_label ("  Add");
    gtk_grid_attach(GTK_GRID(grid), add_button, 5, 0, 1, 1);
    GtkWidget *add_image = gtk_image_new_from_file("add.png");
    gtk_button_set_image(GTK_BUTTON(add_button), add_image);
    gtk_button_set_always_show_image(GTK_BUTTON(add_button), TRUE);
    gtk_button_set_image_position(GTK_BUTTON(add_button), GTK_POS_LEFT);
    g_signal_connect (add_button, "clicked", G_CALLBACK (on_add_clicked), passing_data);
    
    // create a button and add it to the grid //
    change_button = gtk_button_new_with_label ("Change");
    gtk_grid_attach(GTK_GRID(grid), change_button, 6, 0, 1, 1);
    GtkWidget *change_image = gtk_image_new_from_file("change.png");
    gtk_button_set_image(GTK_BUTTON(change_button), change_image);
    gtk_button_set_always_show_image(GTK_BUTTON(change_button), TRUE);
    gtk_button_set_image_position(GTK_BUTTON(change_button), GTK_POS_LEFT);
    g_signal_connect (change_button, "clicked", G_CALLBACK (on_change_clicked), passing_data);

    // create a button and add it to the grid //
    remove_button = gtk_button_new_with_label ("  Remove");
    gtk_grid_attach(GTK_GRID(grid), remove_button, 7, 0, 1, 1);
    GtkWidget *remove_image = gtk_image_new_from_file("remove.png");
    gtk_button_set_image(GTK_BUTTON(remove_button), remove_image);
    gtk_button_set_image_position(GTK_BUTTON(remove_button), GTK_POS_LEFT);
    gtk_button_set_always_show_image(GTK_BUTTON(remove_button), TRUE);
    g_signal_connect (remove_button, "clicked", G_CALLBACK (on_remove_clicked), passing_data);

    gtk_widget_show_all (window);
    gtk_main ();
    free(in_data);
    btcls(tree);
    return 0;
}


gboolean on_key_press (GtkEntry * entry, GdkEventKey *event, gpointer user_data)
{
      data * t_data = user_data;
      char sound_word[30], sound_key[30];
      int i=1;
      int re_val, k = 0, m = 0;
      BTint  val;

      //func to set the position before the first node in the tree to traverse tree
      btpos(t_data->tree, 1);

      int * no_match = &(t_data->no_match);
      char key[100], c;
      char tc[10] = "";
      const gchar * entry_text;
      int * cnt_tab = &t_data->cnt_tab;
      
      //list for drop down
      GtkListStore *ls = gtk_list_store_new(1, G_TYPE_STRING);
      GtkTreeIter iter;
      GtkEntryCompletion *completion = gtk_entry_completion_new(); //create completion dropdown. search gtk_entry_completion

     
      //get text in the entry
      entry_text = gtk_entry_get_text(GTK_ENTRY(entry));
      strcpy(t_data->word, entry_text);
      
      //handle the auto_completion, select suggested words and set to the entry
      if(event->keyval == GDK_KEY_Tab){
          printf("tab key pressed\n");
          if(strcmp(t_data->suggest[0], "") != 0){
              if(*cnt_tab >= 6){
                  *cnt_tab = 0;
              }

              if(strcmp(t_data->suggest[*cnt_tab], "") == 0){
                  *cnt_tab = 0;
              }

              (*cnt_tab)++;

              if(*cnt_tab < 6 && strcmp(t_data->suggest[*cnt_tab - 1], "") != 0 ){
              printf("word found: %s\n", t_data->suggest[*cnt_tab - 1]);
              gchar m_word[30];
              strcpy(m_word, t_data->suggest[*cnt_tab - 1]);
              gtk_entry_set_text (GTK_ENTRY(entry), m_word);
              gtk_editable_set_position(GTK_EDITABLE(entry), -1);
              }

              strcpy(t_data->word, t_data->suggest[*cnt_tab - 1]);
              soundex(t_data->word, sound_word);
              printf("t_data->word: %s\n", t_data->word);

          }
         
       while(i==1){

              //func to get to the next key in the tree, return the key of the node (the word) to the string variable key passed in
              re_val = bnxtky(t_data->tree, key, &val);

              if(re_val==47){
                  printf("no more keys\n");
                  i=0;
              }
              else if(re_val!=0){
                  printf("error occured while select next\n");
                  i=0;
              }
              else {
                  
                  if(strlen(t_data->word) >= 3){
                    if(strncmp(t_data->word, key, strlen(t_data->word) - 2) == 0){
                      if(m<10){
                        soundex(key, sound_key);
                        if(strcmp(sound_word, sound_key) == 0){
                          //add to list, k++
                          printf("key: %s\nsoundex key: %s\n", key, sound_key);
                          gtk_list_store_append(ls, &iter);
                          gtk_list_store_set(ls, &iter, 0, key, -1);
                          m++;
                        }
                      }
                    }
                  }
                  else {
                      if(m<10){
                        //printf("in else\n");
                        soundex(key, sound_key);
                        if(strcmp(sound_word, sound_key) == 0){
                        //add to list, k++
                        gtk_list_store_append(ls, &iter);
                        gtk_list_store_set(ls, &iter, 0, key, -1);
                        m++;
                        }
                      }
                  }
              }
        }
      
      gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(ls));
      gtk_entry_completion_set_match_func(completion, (GtkEntryCompletionMatchFunc)func, NULL, NULL);
      gtk_entry_set_completion(GTK_ENTRY(entry), completion);
      gtk_entry_completion_set_text_column(completion, 0);
      
      return TRUE;
      }

      
      c = gdk_keyval_name (event->keyval)[0];

      if(c>='a'&& c<='z'){
            printf("key pressed: %c\n",c);
            for(int j =0; j<5; j++){
            strcpy(t_data->suggest[j], "");
          }
          tc[0] = c;
          strcat(t_data->word, tc);
          soundex(t_data->word, sound_word);
        
        printf("s: %s\n",t_data->word);

        i = 1;
        m = 0;
        //traverse the tree to find match words for autocomplete and soundex
        while(i==1){
            re_val = bnxtky(t_data->tree, key, &val);

            if(re_val==47){
                printf("no more keys\n");
                i=0;
            }
            else if(re_val!=0){
                printf("error occured while select next\n");
                i=0;
            }
            else {
                if(strncmp(t_data->word, key, strlen(t_data->word)) == 0){

                    if(k<5){
                        strcpy(t_data->suggest[k], key);   
                        printf("suggest %d: %s\n", k, t_data->suggest[k]);
                        k++;
                    }
                }
                if(strlen(t_data->word) >= 3 && strncmp(t_data->word, key, strlen(t_data->word) - 2) == 0){
                  if(m<10){
                    soundex(key, sound_key);
                      if(strcmp(sound_word, sound_key) == 0){
                          //add to list, k++
                          gtk_list_store_append(ls, &iter);
                          gtk_list_store_set(ls, &iter, 0, key, -1);
                          m++;
                      }
                  }
                } 
                else{
                    if(m<10){
                      soundex(key, sound_key);
                        if(strcmp(sound_word, sound_key) == 0){
                          //add to list, k++
                          gtk_list_store_append(ls, &iter);
                          gtk_list_store_set(ls, &iter, 0, key, -1);
                          m++;
                        }
                    }
                }
            }
        }
        gtk_entry_completion_set_model(completion, GTK_TREE_MODEL(ls));
        gtk_entry_completion_set_match_func(completion, (GtkEntryCompletionMatchFunc)func, NULL, NULL);
        gtk_entry_set_completion(GTK_ENTRY(entry), completion);
        gtk_entry_completion_set_text_column(completion, 0);
  }
  
    return FALSE;
}