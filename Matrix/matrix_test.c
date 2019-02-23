#include "matrix.h"
#include "..\Files\files.h"
#include "..\Utilities\utils.h"
#include "..\Hashtable\hashtable.h"
#include "..\Math_Extended\math_extended.h"

int main(void){
    matrix_t* matrix = create_matrix(3, 6);
    double A[] = {9, 2, 4, 1, 3, 0.5};
    matrix->matrix[1] = create_vector_from_array(A, 6);
    matrix->print(matrix);
    printf("About to copy\n");
    matrix_t* m1 = matrix->copy(matrix);
    m1->print(m1);
    printf("Addition\n");
    matrix_t* m2 = matrix_addition(matrix, m1);
    m2->print(m2);
    double ab = m2->get_entry(m2, 1, 0);
    printf("%lf\n", ab);
    m2->set_matrix_row(m2, A, 6, 1);
    m2->set_entry(m2, 2, 1, 9.90);
    m2->row_swap(m2, 1, 2);
    m2->print(m2);

    matrix_t* m3 = m2->transpose(m2);
    printf("Matrix 3\n");
    m3->print(m3);
    double B[] = {2, 1, 1.5};
    matrix_t* m4 = create_matrix(3, 3);
    int i;
    for(i=0; i<3; i++){
        m4->set_matrix_row(m4, B, 3, i);
    }
    printf("Trace: %lf\n", m4->trace(m4));
    m4->print(m4);
    matrix_t* m10 = matrix_hadamard_product(m4, m4);
    m10->print(m10);

    matrix_t* m5 = matrix_multiply(m4, m2);
    printf("Matrix 5\n");
    m5->print(m5);

    vector_t* v = create_vector_from_array(A, 6);
    printf("Norm: %lf\n", v->norm(v));
    v->print(v);
    v->normalise(v);
    v->print(v);
    vector_t* v11 = create_zero_vector(3);
    v11->vector[0] = 3; v11->vector[1] = 1;
    v11->is_zero_vector(v11) ? printf("Is zero vector\n") : printf("Is not zero vector\n");
    printf("Euclidean Distance: %lf\n", vector_euclidean_distance(v11, m4->matrix[1]));
    printf("Manhattan Distance: %lf\n", vector_manhattan_distance(v11, m4->matrix[1]));
    printf("Minkowski Distance: %lf\n", vector_minkowski_distance(v11, m4->matrix[1]));
    printf("Cosine Similarity: %lf\n", cosine_similarity(v11, m4->matrix[1]));
    printf("Arithmetic Mean: %lf\n", v11->arithmetic_mean(v11));
    printf("Geometric Mean: %lf\n", m4->matrix[1]->geometric_mean(m4->matrix[1]));
    v11->print(v11);
    m4->matrix[1]->print(m4->matrix[1]);
    v11->normalise(v11);
    v11->print(v11);
    printf("Grand Sum: %lf\n", m5->grand_sum(m5));
    m4->print(m4);
    matrix_t*m4_square = m4->pow(m4, 3);
    m4_square->print(m4_square);
    m3->set_entry(m3, 3, 1, 4.4);
    m3->set_entry(m3, 3, 2, 14.4);


    double E[] = {-1, 2, 3, -3, 6, 7};
    double F[] = {1, -1, -2, 2, -5, -6};
    double G[] = {-1, 1, 2, -1, 2, 4};
    double H[] = {-2, 2, 4, -2, 4, 8};
    //double I[] = {2, 4, -2};
    //double J[] = {4, 9, -3};
    //double K[] = {-2, -3, 7};

    matrix_t* new_matrix = create_matrix(4, 6);
    new_matrix->set_matrix_row(new_matrix, E, 6, 0);
    new_matrix->set_matrix_row(new_matrix, F, 6, 1);
    new_matrix->set_matrix_row(new_matrix, G, 6, 2);
    new_matrix->set_matrix_row(new_matrix, H, 6, 3);
    printf("New Matrix\n");
    new_matrix->print(new_matrix);
    print_index(new_matrix);
    printf("---\nRank of Matrix: %d\n", new_matrix->rank(new_matrix));
    new_matrix->gaussian_elimination(new_matrix);
    printf("Index Now: ");print_index(new_matrix);
    printf("\n\n\n\n");

    matrix_t* m =csv_to_matrix("test1.txt", ",", lines_in_file("test1.txt"), NULL, 0, 0);
    m->print(m);
    int lines = lines_in_file("test1.txt");
    printf("Lines: %d\n", lines);

    //double Z[] = {1, 3, 4, 7, 8};
    //vector_t* z = create_vector_from_array(Z, 5);
    //printf("%lf\n", z->standard_deviation(z, POPULATION));
    //printf("Std 0: %lf\n", m->matrix[0]->standard_deviation(m->matrix[0], POPULATION));
    //printf("Std 1: %lf\n", m->matrix[1]->standard_deviation(m->matrix[1], POPULATION));
    matrix_t* m14 = matrix_to_corrcoef(m, POPULATION);
    printf("Correlation coefficient matrix complete\n");

    m14->print(m14);
    m14->to_csv(m14, "doggo.txt");
    matrix_t* m99 = csv_to_matrix("ATP_matches_mod.csv", ",", lines_in_file("ATP_matches_mod.csv"), NULL, LABELLED, LABELLED);
    printf("Matrix loaded in\n");
    printf("Dimension: %d x %d\n", m99->num_rows, m99->num_columns);
    m99->print_head(m99, 10);
    printf("Column Names Activated: %d\n", m99->column_index_used);
    print_column_names(m99);

    printf("About to create hashtable\n");
    hashtable_t* winner = create_simple_hashtable(500, STRING);
    hashtable_t* loser = create_simple_hashtable(500, STRING);
    for(i=0; i<m99->num_rows; i++){
        char* copy = copy_string(m99->index_str[i]);
        char* token = strtok(copy, "~");
        winner->insert(winner, ((void*)token), NULL);
        token = strtok(NULL, "~");
        loser->insert(loser, ((void*)token), NULL);
    }
    printf("About to print frequencies for winner\n");
    dict_array_t* a = loser->to_array(loser);
    hashtable_t* player = create_simple_hashtable(500, STRING);
    for(i=0; i<a->num_elements; i++){
        double wins = winner->get_frequency(winner, a->key[i]);
        double losses = loser->get_frequency(loser, a->key[i]);
        //printf("%s's W/L is %lf%%\n", ((char*)a->key[i]), 100*(wins/(wins+losses)));
        player->insert(player, a->key[i], doub(100*(wins/(wins+losses))));
    }
    printf("Num Elements: %d\n", player->len(player));
    printf("DONEDONEDONE\n");
    double log_loss_sum = 0.0;
    int correct = 0;
    int incorrect = 0;
    int total = 0;
    int j;
    double constant = -.1;
    for(j=0; j<50; j++){
        log_loss_sum = 0.0;
        correct = 0;
        incorrect = 0;
        total = 0;
        for(i=0; i<m99->num_rows; i++){
            char* copy = copy_string(m99->index_str[i]);
            char* token = strtok(copy, "~");
            //char* win = token;
            double* winner_rate = player->in(player, token);
            

            token = strtok(NULL, "~");
            double* loser_rate = player->in(player, token);
            
            
            //int winner_rank = get_matrix_entry_by_colname(m99, i, "Winner_Rank");
            //int loser_rank = get_matrix_entry_by_colname(m99, i, "Loser_Rank");
            if (winner_rate == NULL || loser_rate == NULL){
                continue;
            }
            double win_rate = *winner_rate;
            double lose_rate = *loser_rate;
            int winner;
            (win_rate > lose_rate) ? (winner = 0) : (winner = 1);
            (win_rate > 0.5) ? (win_rate += constant) : win_rate;
        if (winner){
                total++;
                incorrect++;
                //printf("Upset: %s (%d) (%lf%%) > %s (%d) (%lf%%); Difference: %lf%%\n", win, winner_rank, *winner_rate, token, loser_rank,  *loser_rate, *loser_rate - *winner_rate);
                log_loss_sum += log_loss(0, win_rate/100);
                
            }
            else{
                total++;
                correct++;
                log_loss_sum += log_loss(1, win_rate/100);
            }
            free(copy);
        }
    //winner->print_frequencies(winner, 30);
    
    printf("Win Ratio Log loss sum (%lf): %lf\n", log_loss_sum/total, constant);
    constant += .01;
    }
    printf("Win Ratio Method: Total Games: %d, Accuracy: %lf\n", total, (correct + 0.0)/total);

    correct = 0;
    incorrect = 0;
    total = 0;
    log_loss_sum = 0.0;
    constant = 1;
    for(j=0; j<50; j++){
        log_loss_sum = 0.0;
        correct = 0;
        incorrect = 0;
        total = 0;
        double probability = 0.0;
        for(i=0; i<m99->num_rows; i++){
            int winner_rank = get_matrix_entry_by_colname(m99, i, "Winner_Rank");
            int loser_rank = get_matrix_entry_by_colname(m99, i, "Loser_Rank");
            int winner = winner_rank > loser_rank;
             probability = 0.5 + (abs(winner_rank-loser_rank)/(constant*100));
            if (winner){
                total++;
                incorrect++;
                log_loss_sum += log_loss(0, probability);
            }
            else{
                total++;
                correct++;
                log_loss_sum += log_loss(1, probability);
            }
        }
        
        printf("Ranking Method Log loss sum: %lf, %lf\n", log_loss_sum/total, constant);
        constant+= 1.0;
    }
    printf("Ranking Method: Total Games: %d, Accuracy: %lf\n", total, (correct + 0.0)/total);
    //loser->print_frequencies(loser, 50);
    printf("Winner: %d\n", winner->len(winner));
    printf("Loser: %d\n", loser->len(loser));
    printf("Players: %d\n", hashtable_union_len(winner, loser, STRING));
}