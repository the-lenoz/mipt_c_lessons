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


int _CircularList_init(_CircularList *list, size_t initial_capacity);
int _CircularList_destroy(_CircularList *list);

ListStatusType _CircularList_verify(_CircularList *list);
int _CircularList_dump(_CircularList *list, FILE* fp);
int _CircularList_dump_graph(_CircularList *list, FILE *fp);

static int _CircularList_fit_size(_CircularList* list);
static int _CircularList_is_fit_size(_CircularList* list);
static size_t _CircularList_get_enough_size(_CircularList* list);



ListStatusType _CircularList_insert_after(_CircularList *list, size_t phys_index, list_elem_type element, size_t *dst_phys_index);
ListStatusType _CircularList_insert_before(_CircularList *list, size_t phys_index, list_elem_type element, size_t *dst_phys_index);
ListStatusType _CircularList_delete(_CircularList *list, size_t phys_index);

ListStatusType _CircularList_get(_CircularList *list, size_t phys_index, list_elem_type *dst);


int _CircularList_init(_CircularList *list, size_t initial_capacity)
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

int _CircularList_destroy(_CircularList *list)
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


ListStatusType _CircularList_verify(_CircularList *list)
{
    ListStatusType status = LIST_OKAY;

    if (list == NULL)
    {
        status = LIST_STRUCTURE_INVALID;
    } 
    
    // TODO

    return status;
}

int _CircularList_dump(_CircularList *list, FILE *fp)
{
    assert(fp != NULL);

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
    
    _CircularList_dump_graph(list, fp);
            
    fprintf(fp, HTML_DUMP_END);

    return 0;
}

int _CircularList_dump_graph(_CircularList *list, FILE *fp)
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
    
    agset(g, "rankdir", "TB");

    agset(g, "bgcolor", "#EFEFEF"); 
    agset(g, "label", "CircularList DUMP"); 
    
    agattr(g, AGNODE, "shape", "hexagon"); 
    agattr(g, AGNODE, "style", "filled"); 
    agattr(g, AGNODE, "fillcolor", "red"); 
    agattr(g, AGEDGE, "color", "gray40"); 
    agattr(g, AGEDGE, "fontname", "Arial"); 

    _ListNode node = {};
    Agnode_t **graph_nodes = (Agnode_t**)calloc(list->capacity, sizeof(Agnode_t*));

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
    }

    
    
    Agedge_t *edge = NULL;
    for (size_t i = 0; i < list->capacity; ++i)
    {
        node = list->data[i];
        if (i < list->capacity - 1)
        {
            edge = agedge(g, graph_nodes[i], graph_nodes[i + 1], NULL, 1);
            agset(edge, "color", "#ffffff");
        }

        if ((size_t)node.next_phys_index < list->capacity)
        {
            edge = agedge(g, graph_nodes[i], graph_nodes[(size_t)node.next_phys_index], NULL, 1);
            agset(edge, "color", "green");
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
    
    int render_result = gvRenderData(gvc, g, "svg", &svg_data, &length);
    
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


int _CircularList_fit_size(_CircularList* list)
{
    ListStatusType status = _CircularList_verify(list);
    if (status != LIST_OKAY)
    {
        return -1;
    }

    if (!_CircularList_is_fit_size(list))
    {
        size_t old_capacity = list->capacity;
        size_t new_capacity = _CircularList_get_enough_size(list);
        printf("RESIZING! %zu\n", new_capacity * sizeof(_ListNode));
        _ListNode* list_data_backup = list->data;

        list->data = (_ListNode*) realloc(list->data, new_capacity * sizeof(_ListNode));
        
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

static int _CircularList_is_fit_size(_CircularList* list)
{
    return list->size <= list->capacity - 3;
}

static size_t _CircularList_get_enough_size(_CircularList* list)
{
    return list->size * 2 + 3;
}


ListStatusType _CircularList_insert_after(_CircularList *list, size_t phys_index, list_elem_type element, size_t *dst_phys_index)
{
    assert(list != NULL);
    assert(dst_phys_index != NULL);

    ListStatusType list_status = _CircularList_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (_CircularList_fit_size(list) == -1)
    {
        return LIST_STRUCTURE_INVALID;
    }

    if (phys_index < 0 || phys_index >= list->size)
    {
        return LIST_INDEX_INVALID;
    }

    *dst_phys_index = list->free_head_phys_index;

    //if ((ssize_t)phys_index == list->head_phys_index)
    //{
    //    list->head_phys_index = *dst_phys_index;
    //}

    list->free_head_phys_index = list->data[list->free_head_phys_index].next_phys_index;


    list->data[*dst_phys_index] = {};

    list->data[*dst_phys_index].next_phys_index=list->data[phys_index].next_phys_index;
    list->data[*dst_phys_index].prev_phys_index=phys_index;
    list->data[*dst_phys_index].value=element;

    list->data[list->data[phys_index].next_phys_index].prev_phys_index = *dst_phys_index; // Перешиваем
    list->data[phys_index].next_phys_index = *dst_phys_index; 

    list->size++;

    return list_status;
}

ListStatusType _CircularList_insert_before(_CircularList *list, size_t phys_index, list_elem_type element, size_t *dst_phys_index)
{
    assert(list != NULL);
    assert(dst_phys_index != NULL);


    ListStatusType list_status = _CircularList_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (_CircularList_fit_size(list) == -1)
    {
        return LIST_STRUCTURE_INVALID;
    }

    if (phys_index < 0 || phys_index > list->size)
    {

        return LIST_INDEX_INVALID;
    }

    *dst_phys_index = list->free_head_phys_index;

    list->free_head_phys_index = list->data[list->free_head_phys_index].next_phys_index;


    list->data[*dst_phys_index] = {};

    list->data[*dst_phys_index].next_phys_index=phys_index;
    list->data[*dst_phys_index].prev_phys_index=list->data[phys_index].prev_phys_index;
    list->data[*dst_phys_index].value=element;

    list->data[list->data[phys_index].prev_phys_index].next_phys_index = *dst_phys_index; // Перешиваем
    list->data[phys_index].prev_phys_index = *dst_phys_index;

    list->size++;


    return list_status;
}

ListStatusType _CircularList_delete(_CircularList *list, size_t phys_index)
{
    assert(list!= NULL);

    ListStatusType list_status = _CircularList_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (_CircularList_fit_size(list) == -1)
    {
        return LIST_STRUCTURE_INVALID;
    }

    if (phys_index >= list->size)
    {
        return LIST_INDEX_INVALID;
    }

    //if ((ssize_t)phys_index == list->head_phys_index)
    //{
    //    list->head_phys_index = list->data[phys_index].next_phys_index;
    //}

    list->data[list->data[phys_index].prev_phys_index].next_phys_index = list->data[phys_index].next_phys_index; // Перешиваем
    list->data[list->data[phys_index].next_phys_index].prev_phys_index = list->data[phys_index].prev_phys_index;

    list->data[phys_index].next_phys_index = list->free_head_phys_index;
    list->data[phys_index].prev_phys_index = -1;
    list->data[phys_index].value = NAN;
    
    list->free_head_phys_index = phys_index;

    list->size--;

    

    return list_status;
}


ListStatusType _CircularList_get(_CircularList *list, size_t phys_index, _ListNode *dst)
{
    assert(list != NULL);

    ListStatusType list_status = _CircularList_verify(list);
    if (list_status != LIST_OKAY)
    {
        return list_status;
    }

    if (_CircularList_fit_size(list) == -1)
    {
        return LIST_STRUCTURE_INVALID;
    }

    if (phys_index >= list->size)
    {
        return LIST_INDEX_INVALID;
    }

    if (list->data[phys_index].prev_phys_index == -1)
    {
        return LIST_INDEX_INVALID;
    }

    *dst = list->data[phys_index];

    return list_status;
}



IteratorStatusType CircularListIterator_init(CircularListIterator *iter)
{
    assert(iter != NULL);

    *iter = {};
    
    iter->list = (_CircularList*) calloc(1, sizeof(*iter->list));

    if (iter->list == NULL)
    {
        return ITERATOR_INIT_ERROR;
    }

    if (_CircularList_init(iter->list, INITIAL_LIST_CAPACITY) == -1)
    {
        return ITERATOR_INIT_ERROR;
    }

    if (_CircularList_get(iter->list, 0, &iter->last_element) != LIST_OKAY)
    {
        return ITERATOR_INIT_ERROR;
    }

    srand(time(NULL));

    iter->last_iteration_status = ITERATOR_OK;


    return ITERATOR_OK;
}
IteratorStatusType CircularListIterator_destroy(CircularListIterator *iter)
{
    IteratorStatusType status = ITERATOR_OK;

    if (iter == NULL)
    {
        return ITERATOR_INVALID;
    }

    if (iter->list != NULL)
    {
        _CircularList_destroy(iter->list);
        free(iter->list);
    }
    else
    {
        status = ITERATOR_INVALID;
    }

    *iter = {};

    return status;
}

IteratorStatusType CircularListIterator_verify(CircularListIterator *iter)
{
    if (iter == NULL ||
        iter->list == NULL ||
        _CircularList_verify(iter->list) != LIST_OKAY)
    {
        return ITERATOR_INVALID;
    }

    // TODO (check hash)

    return ITERATOR_OK;
}
IteratorStatusType CircularListIterator_dump(CircularListIterator *iter, const char *directory)
{
    assert(directory != NULL);

    char log_file_path[MAX_FILE_PATH_LEN + 1] = {};

    snprintf(log_file_path, MAX_FILE_PATH_LEN, "%s/log_%lu_%d.html", directory, (unsigned long)time(NULL), rand());

    FILE* fp = fopen(log_file_path, "w");

    if (iter == NULL)
    {
        fprintf(fp, "CircularListIterator (NULL)\n");
    }
    else 
    {
        _CircularList_dump(iter->list, fp);
    }

    fclose(fp);

    return CircularListIterator_verify(iter);
}

list_elem_type CircularListIterator_next(CircularListIterator *iter)
{
    assert(iter != NULL);
    IteratorStatusType status = CircularListIterator_verify(iter);
    if (status != ITERATOR_OK)
    {
        iter->last_iteration_status = status;
        return {};
    }
    _ListNode node = {};
    ListStatusType list_status = _CircularList_get(iter->list, iter->last_element.next_phys_index, &node);

    if (list_status != LIST_OKAY)
    {
        iter->last_iteration_status = ITERATOR_STOP;
        return {};
    }

    iter->last_element = node;

    return node.value;
}
list_elem_type CircularListIterator_prev(CircularListIterator *iter)
{
    assert(iter != NULL);
    IteratorStatusType status = CircularListIterator_verify(iter);
    if (status != ITERATOR_OK)
    {
        iter->last_iteration_status = status;
        return {};
    }
    _ListNode node = {};
    ListStatusType list_status = _CircularList_get(iter->list, iter->last_element.prev_phys_index, &node);

    if (list_status != LIST_OKAY)
    {
        iter->last_iteration_status = ITERATOR_STOP;
        return {};
    }

    iter->last_element = node;

    return node.value;
}


IteratorStatusType CircularListIterator_insert_after(CircularListIterator *iter, list_elem_type element)
{
    assert(iter != NULL);
    IteratorStatusType status = CircularListIterator_verify(iter);
    if (status != ITERATOR_OK)
    {
        iter->last_iteration_status = status;
        return {};
    }


    ListStatusType list_status = LIST_OKAY;
    size_t dst_phys_index = 0;
    list_status = _CircularList_insert_before(iter->list, iter->last_element.next_phys_index, element, &dst_phys_index);

    if (list_status != LIST_OKAY)
    {
        iter->last_iteration_status = ITERATOR_STOP;
        return {};
    }

    return status;
}
IteratorStatusType CircularListIterator_insert_before(CircularListIterator *iter, list_elem_type element)
{
    assert(iter != NULL);
    IteratorStatusType status = CircularListIterator_verify(iter);
    if (status != ITERATOR_OK)
    {
        iter->last_iteration_status = status;
        return {};
    }


    ListStatusType list_status = LIST_OKAY;
    size_t dst_phys_index = 0;
    list_status = _CircularList_insert_after(iter->list, iter->last_element.prev_phys_index, element, &dst_phys_index);

    if (list_status != LIST_OKAY)
    {
        iter->last_iteration_status = ITERATOR_STOP;
        return {};
    }

    return status;
}

IteratorStatusType CircularListIterator_delete_next(CircularListIterator *iter)
{
    assert(iter != NULL);
    IteratorStatusType status = CircularListIterator_verify(iter);
    if (status != ITERATOR_OK)
    {
        iter->last_iteration_status = status;
        return {};
    }


    ListStatusType list_status = LIST_OKAY;
    list_status = _CircularList_delete(iter->list, iter->last_element.next_phys_index);

    if (list_status != LIST_OKAY)
    {
        iter->last_iteration_status = ITERATOR_STOP;
        return {};
    }

    return status;
}
IteratorStatusType CircularListIterator_delete_prev(CircularListIterator *iter)
{
    assert(iter != NULL);
    IteratorStatusType status = CircularListIterator_verify(iter);
    if (status != ITERATOR_OK)
    {
        iter->last_iteration_status = status;
        return {};
    }


    ListStatusType list_status = LIST_OKAY;
    list_status = _CircularList_delete(iter->list, iter->last_element.prev_phys_index);

    if (list_status != LIST_OKAY)
    {
        iter->last_iteration_status = ITERATOR_STOP;
        return {};
    }

    return status;
}

