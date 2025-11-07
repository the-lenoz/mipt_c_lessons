#include "my_assert.hpp"

#include "list.hpp"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <ctime>
#include <graphviz/cgraph.h>
#include <graphviz/gvc.h>

#define INITIAL_LIST_CAPACITY   32
#define LIST_CANARY 3.141592682


#define HTML_DUMP_START_FORMAT  "<h2>%s</h2><br><pre>"
#define HTML_DUMP_END           "</pre>"

#define HTML_RED                "<div style=\"color: red\">%s</div>"

#define MAX_NODE_NAME_LEN   32
#define MAX_NODE_LABEL_LEN  256

#define MAX_FILE_PATH_LEN   512


static int CircularList_fit_size(CircularList *list);
static int CircularList_is_fit_size(CircularList *list);
static size_t CircularList_get_enough_size(CircularList *list);

static int CircularList_check_iterator(CircularList *list, iterator_t *iter);


int CircularList_init(CircularList *list, size_t initial_capacity)
{
    assert(list != NULL);

    *list = {};
    list->capacity = initial_capacity;
    list->data = (_ListNode*) calloc(initial_capacity, sizeof(_ListNode));

    if (list->data == NULL)
    {
        return -1;
    }

    list->size = 0;

    list->free_head_phys_index = 1;

    list->data[0].next_phys_index = 0;
    list->data[0].prev_phys_index = 0;
    list->data[0].value = LIST_CANARY;

    for (size_t i = 1; i < list->capacity; ++i)
    {
        list->data[i].next_phys_index = i + 1;
        list->data[i].prev_phys_index = -1;
        list->data[i].value = NAN; // !FOR DOUBLE ONLY!
    }

    return 0;
}

int CircularList_destroy(CircularList *list)
{
    if (list == NULL)
    {
        return -1;
    }

    if (list->data != NULL)
    {
        free(list->data);
    }

    *list = {};

    return 0;
}


ListStatusType CircularList_verify(CircularList *list)
{
    ListStatusType status = LIST_OKAY;

    if (list == NULL)
    {
        status = LIST_STRUCTURE_INVALID;
    } 
    
    // TODO

    return status;
}

int CircularList_dump(CircularList *list, const char *directory)
{
    assert(directory != NULL);

    char log_file_path[MAX_FILE_PATH_LEN + 1] = {};

    snprintf(log_file_path, MAX_FILE_PATH_LEN, "%s/log_%lu_%d.html", directory, (unsigned long)time(NULL), rand());

    FILE *fp = fopen(log_file_path, "w");

    fprintf(fp, HTML_DUMP_START_FORMAT, "Circular list dump");

    if (list == NULL)
    {
        fprintf(fp, HTML_RED, "List = NULL\n");
        fprintf(fp, HTML_DUMP_END);
        return -1;
    }
    
    fprintf(fp, "List\n  data={\n");
    for (size_t i = 0; i < list->capacity; ++i)
    {
        fprintf(fp, "    ListNode(value=%lf,\tnext_index=%ld,\tprev_index=%ld),",
                list->data[i].value, list->data[i].next_phys_index, list->data[i].prev_phys_index);
        if (list->data[i].prev_phys_index == -1)
        {
            fprintf(fp, " <--- GARBAGE");
        }
        fprintf(fp, "\n");
    }
    fprintf(fp, "  },\n");
    fprintf(fp, "  free_head_phys_index=%ld,\n"
                "  capacity=%zu,\n"
                "  size=%zu\n}\n\n", list->free_head_phys_index, list->capacity, list->size);
    
    CircularList_dump_graph(list, fp);
            
    fprintf(fp, HTML_DUMP_END);

    fclose(fp);

    return 0;
}

int CircularList_dump_graph(CircularList *list, FILE *fp)
{
    assert(fp != NULL);

    if (list == NULL || list->data == NULL) 
    {
        fprintf(fp, HTML_RED, "List/list->data is NULL\n");
        return -1;
    }

    GVC_t *gvc = gvContext();

    if (!gvc) {
        fprintf(fp, HTML_RED, "Error initializing GVC\n");
        return -1;
    }

    Agraph_t *g = agopen("CircularList_dump", Agdirected, NULL);
    if (!g) {
        fprintf(fp, HTML_RED, "Error creating graph\n");
        return -1;
    }
    
    agset(g, "rankdir", "LR");

    agset(g, "bgcolor", "#EFEFEF"); 
    agset(g, "label", "CircularList DUMP"); 
    
    agattr(g, AGNODE, "shape", "hexagon"); 
    agattr(g, AGNODE, "style", "filled"); 
    agattr(g, AGNODE, "fillcolor", "red"); 
    agattr(g, AGEDGE, "color", "gray40"); 
    agattr(g, AGEDGE, "fontname", "Arial"); 
    
    _ListNode node = {};
    Agnode_t **graph_nodes = (Agnode_t**)calloc(list->capacity, sizeof(Agnode_t*));

    Agraph_t *samerank_g = agsubg(g, "same_rank", 1);
    agset(samerank_g, "rank", "same");

    
    char node_label_str[MAX_NODE_LABEL_LEN + 1] = {};
    char node_name_str[MAX_NODE_NAME_LEN + 1] = {};

    for (size_t i = 0; i < list->capacity; ++i)
    {
        node = list->data[i];
        snprintf(node_name_str, MAX_NODE_NAME_LEN, "node%zu", i);
        snprintf(node_label_str, MAX_NODE_LABEL_LEN, "{ index = %zu | data = %lf | {<p> prev = %ld | <n> next = %ld} }",
                i, node.value, node.prev_phys_index, node.next_phys_index);   
        graph_nodes[i] = agnode(g, node_name_str, 1);
        agset(graph_nodes[i], "label", node_label_str);
        agset(graph_nodes[i], "shape", "record");
        agset(graph_nodes[i], "fillcolor", i > 0 ? (node.prev_phys_index == -1 ? "magenta" : "#777777" ) : "orange");
        agsubnode(samerank_g, graph_nodes[i], 1);
    }

    
    
    Agedge_t *edge = NULL;
    for (size_t i = 0; i < list->capacity; ++i)
    {
        node = list->data[i];
        if (i < list->capacity - 1)
        {
            edge = agedge(g, graph_nodes[i], graph_nodes[i + 1], NULL, 1);
            agset(edge, "color", "#ffffff");
	    agset(edge, "weight", "100");
        }

        if ((size_t)node.next_phys_index < list->capacity)
        {
            edge = agedge(g, graph_nodes[i], graph_nodes[(size_t)node.next_phys_index], NULL, 1);
            agset(edge, "color", "green");
	    agset(edge, "weight", "2");
        }
        else
        {
            snprintf(node_name_str, MAX_NODE_NAME_LEN, "%ld", node.next_phys_index);
            edge = agedge(g, graph_nodes[i], agnode(g, node_name_str, 1), NULL, 1);
        }

        if ((size_t)node.prev_phys_index < list->capacity)
        {
            edge = agedge(g, graph_nodes[i], graph_nodes[(size_t)node.prev_phys_index], NULL, 1);
            agset(edge, "color", "yellow");
	    agset(edge, "weight", "2");
        }
        else if (node.prev_phys_index != -1)
        {
            snprintf(node_name_str, MAX_NODE_NAME_LEN, "%ld", node.prev_phys_index);
            edge = agedge(g, graph_nodes[i], agnode(g, node_name_str, 1), NULL, 1);
        }
    }

    Agnode_t *free_head = agnode(g, "free", 1);
    agset(free_head, "fillcolor", "pink");
    agedge(g, free_head, graph_nodes[list->free_head_phys_index], NULL, 1);

    const char *layout_engine = "dot";
    int layout_result = gvLayout(gvc, g, layout_engine);
    
    if (layout_result != 0) {
        fprintf(fp, HTML_RED, "Error in gvLayout\n");
        agclose(g);
        gvFreeContext(gvc);
        return -1;
    }

    char *svg_data = NULL;
    size_t length = 0;
    
    int render_result = gvRenderData(gvc, g, "dot", &svg_data, &length);
    
    gvFreeLayout(gvc, g);


    if (render_result == 0 && svg_data) {
        fprintf(fp, "%s", svg_data);
        gvFreeRenderData(svg_data);
    } else {
        fprintf(fp, HTML_RED, "Error in gvRenderData\n");
    }

    agclose(g);
    gvFreeContext(gvc);

    return 0;
}


int CircularList_fit_size(CircularList *list)
{
    ListStatusType status = CircularList_verify(list);
    if (status != LIST_OKAY)
    {
        return -1;
    }

    if (!CircularList_is_fit_size(list))
    {
        size_t old_capacity = list->capacity;
        size_t new_capacity = CircularList_get_enough_size(list);
        //printf("RESIZING! %zu\n", new_capacity  *sizeof(_ListNode));
        _ListNode *list_data_backup = list->data;

        list->data = (_ListNode*) realloc(list->data, new_capacity *sizeof(_ListNode));
        
        list->capacity = new_capacity;

        if (list->data == NULL)
        {
            list->data = list_data_backup;
            return -1;
        }

        for (size_t i = old_capacity - 1; i < list->capacity; ++i)
        {
            list->data[i].next_phys_index = i + 1;
            list->data[i].prev_phys_index = -1;
            list->data[i].value = NAN; // !FOR DOUBLE ONLY!
        }

    }
    return 0;
}

static int CircularList_is_fit_size(CircularList *list)
{
    return list->size <= list->capacity - 3;
}

static size_t CircularList_get_enough_size(CircularList *list)
{
    return list->size  *2 + 3;
}


static inline int CircularList_check_iterator(CircularList *list, iterator_t *iter)
{
    return list != NULL && iter != NULL && ((size_t)(*iter) <= list->size);
}


ListStatusType CircularList_copy_iterator(CircularList *list, iterator_t *src, iterator_t *dst)
{
    assert(list);
    assert(src);
    assert(dst);
    ListStatusType status = CircularList_verify(list);
    if (status != LIST_OKAY) return status;

    *dst = *src;

    return LIST_OKAY;
}

iterator_t CircularList_get_null_iterator(CircularList* list)
{
    assert(list);
    return 0;
}



ListStatusType CircularList_insert_after(CircularList *list, iterator_t *iter, list_elem_type element)
{
    assert(list != NULL);
    assert(iter != NULL);

    ListStatusType list_status = CircularList_verify(list);
    if (list_status != LIST_OKAY) return list_status;

    CircularList_fit_size(list);

    if (!CircularList_check_iterator(list, iter))
    {
        return LIST_INDEX_INVALID;
    }

    size_t index = list->free_head_phys_index;

    list->free_head_phys_index = list->data[list->free_head_phys_index].next_phys_index;


    list->data[index] = {};

    list->data[index].next_phys_index=list->data[*iter].next_phys_index;
    list->data[index].prev_phys_index=*iter;
    list->data[index].value=element;

    list->data[list->data[*iter].next_phys_index].prev_phys_index = index; // Перешиваем
    list->data[*iter].next_phys_index = index; 

    list->size++;

    return list_status;
}

ListStatusType CircularList_delete(CircularList *list, iterator_t *iter)
{
    assert(list);
    assert(iter);

    ListStatusType list_status = CircularList_verify(list);
    if (list_status != LIST_OKAY) return list_status;

    CircularList_fit_size(list);

    if (!CircularList_check_iterator(list, iter) || *iter == 0) return LIST_INDEX_INVALID;


    list->data[list->data[*iter].prev_phys_index].next_phys_index = list->data[*iter].next_phys_index; // Перешиваем
    list->data[list->data[*iter].next_phys_index].prev_phys_index = list->data[*iter].prev_phys_index;

    list->data[*iter].next_phys_index = list->free_head_phys_index;
    list->data[*iter].prev_phys_index = -1;
    list->data[*iter].value = NAN;
    
    list->free_head_phys_index = *iter;

    list->size--;    

    return list_status;
}


list_elem_type CircularList_next(CircularList *list, iterator_t *iter, ListStatusType *status)
{
    assert(list);
    assert(iter);

    ListStatusType st = LIST_OKAY;
    status = status ? status : &st;

    if ((*status = CircularList_verify(list)) != LIST_OKAY) return NAN; // ONLY DOUBLE

    if (!CircularList_check_iterator(list, iter))
    {
        *status = LIST_INDEX_INVALID;
	return NAN;
    }

    *iter = list->data[*iter].next_phys_index;

    return list->data[*iter].value;
}

list_elem_type CircularList_prev(CircularList *list, iterator_t *iter, ListStatusType *status)
{
    assert(list);
    assert(iter);

    ListStatusType st = LIST_OKAY;
    status = status ? status : &st;

    if ((*status = CircularList_verify(list)) != LIST_OKAY) return NAN; // ONLY DOUBLE

    if (!CircularList_check_iterator(list, iter))
    {
        *status = LIST_INDEX_INVALID;
	return NAN;
    }

    *iter = list->data[*iter].prev_phys_index;

    return list->data[*iter].value;
}

/*
IteratorStatusType CircularListIterator_dump(CircularListIterator *iter, const char *directory)
{
    assert(directory != NULL);

    char log_file_path[MAX_FILE_PATH_LEN + 1] = {};

    snprintf(log_file_path, MAX_FILE_PATH_LEN, "%s/log_%lu_%d.html", directory, (unsigned long)time(NULL), rand());

    FILE *fp = fopen(log_file_path, "w");

    if (iter == NULL)
    {
        fprintf(fp, "CircularListIterator (NULL)\n");
    }
    else 
    {
        CircularList_dump(iter->list, fp);
    }

    fclose(fp);

    return CircularListIterator_verify(iter);
}*/
