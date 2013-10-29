
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pattern.h"

#define l_2_r   2
#define r_2_l   3
#define u_2_d   0
#define d_2_u   1
#define lu_2_rd 4
#define ld_2_ru 6
#define ru_2_ld 5
#define rd_2_lu 7

#define Size 100

/*�������*/
pos_t transformation2[8] = {
  { 1,  0}, /* from left to right */

  {-1,  0}, /* from right to left */

  { 0,  1}, /* from up to down */
  
  { 0, -1}, /* from down to up */

  { 1,  1}, /* from left up corner to right down corner*/

  { 1, -1}, /* from left down corner to right up corner*/
  
  {-1,  1}, /* from right up corner to left down corner*/
  
  {-1, -1}  /* from right down corner to left up corner*/
};

struct sim_type{
    int x, y;
    int dfa;
}simple[100];

int count, sim_c, my_color;
FILE *patin = NULL;

CDFA::CDFA()
{}

int CDFA::change(int Color)
{
    if ( Color == 1 )
        return Color+my_color;
    if ( Color == 2 )
        return Color-my_color;
    return Color;
}

void CDFA::copy_board(char ori_board[][GRID_NUM], char temp_board[][GRID_NUM])
{
    int i, j;
    for (i=0; i<GRID_NUM; i++){
        for (j=0; j<GRID_NUM; j++){
            temp_board[i][j] = ori_board[i][j];
        }
    }
}

int CDFA::check(move_t bestMove[], move_t now)
{
    int i;
    for (i=0; i<count; i++){
        if (now.positions[0].x == bestMove[i].positions[0].x && 
            now.positions[1].x == bestMove[i].positions[1].x &&
            now.positions[0].y == bestMove[i].positions[0].y &&
            now.positions[1].y == bestMove[i].positions[1].y)
            return 0;
        if (now.positions[0].x == bestMove[i].positions[1].x && 
            now.positions[1].x == bestMove[i].positions[0].x &&
            now.positions[0].y == bestMove[i].positions[1].y &&
            now.positions[1].y == bestMove[i].positions[0].y)
            return 0;
    }
    return 1;
}

void CDFA::new_match2(pos_t point, dfa_t *pdfa, move_t bestMove[], int direction)
{
    int p=0, j, step;
    for (step=0; step<2*pdfa->last_state; step++){
        p = pdfa->states[p].next[change(m_board[point.x][point.y])];   
        if (p == pdfa->last_state){
            for (j=0; j<pdfa->last_index; j++){

                if ( pdfa->indexes[j].mode == 1 ){

                    bestMove[count].positions[0].x = simple[sim_c-1].x;
                    bestMove[count].positions[0].y = simple[sim_c-1].y;
                    bestMove[count].positions[1].x = pdfa->indexes[j].offset[0] * (-transformation2[direction].x) + point.x;
                    bestMove[count].positions[1].y = pdfa->indexes[j].offset[0] * (-transformation2[direction].y) + point.y;
                    if (check(bestMove, bestMove[count]))
                        count++;
                }
            }
            p = 0;
        }
        point.x += transformation2[direction].x;
        point.y += transformation2[direction].y;
        if (point.x > 19 || point.y > 19 || point.x < 1 || point.y < 1)
            return;
    }
}

void CDFA::new_match(pos_t point, move_t bestMove[], int ori_direction)
{
    int i, direction;
    pos_t temp;
    for (i=0; i<m_dfa_index; i++){
        for (direction=0; direction<8; direction++){
            if (direction == ori_direction || 
                (transformation2[direction].x == -transformation2[ori_direction].x && 
                transformation2[direction].y == -transformation2[ori_direction].y))
                continue;
            temp.x = point.x - (transformation2[direction].x * (m_dfa_array[i].last_state-1));
            temp.y = point.y - (transformation2[direction].y * (m_dfa_array[i].last_state-1));
            if (temp.x > 19)
                temp.x = 19;
            if (temp.y > 19)
                temp.y = 19;
            if (temp.x < 1)
                temp.x = 1;
            if (temp.y < 1)
                temp.y = 1;
            new_match2(temp, &m_dfa_array[i], bestMove, direction);
        }
    }
}

void CDFA::addpoint(move_t bestMove[], pos_t point)
{
    int i, j, k;
    for (i=1; i<=19; i++){
        for (j=1; j<=19; j++){
            if (m_board[i][j] == 0){ 
                for (k=0; k<8; k++){
                    if (m_board[i+transformation2[k].x][j+transformation2[k].y] == 1 || 
                        m_board[i+transformation2[k].x][j+transformation2[k].y] == 2 ||
                        (i != 1 && i != 19 && j != 1 && j != 19 &&
                        (m_board[i+2*transformation2[k].x][j+2*transformation2[k].y] == 1 || 
                        m_board[i+2*transformation2[k].x][j+2*transformation2[k].y] == 2))){
                              break;
                    }
                }
                if (k < 8){
                    bestMove[count].positions[0].x = point.x;
                    bestMove[count].positions[0].y = point.y;
                    bestMove[count].positions[1].x = i;
                    bestMove[count].positions[1].y = j;
                    if (check(bestMove, bestMove[count]))
                        count++;
                }
            }
        }
    }
}

/*ƥ���Ӻ���*/
void CDFA::match2(pos_t point, dfa_t *pdfa, move_t bestMove[], int direction, int dfa_num)
{
    int p=0, j;
    pos_t temp, temp_bestMove;
    while (1){
        p = pdfa->states[p].next[change(m_board[point.x][point.y])];     
        if (p == pdfa->last_state){
            for (j=0; j<pdfa->last_index; j++){
                /*��Ϊĳһ�������ӹ���˫����,������з�*/
                if ( pdfa->indexes[j].mode == 2 ){
                    if (pdfa->indexes[j].offset[0] != pdfa->indexes[j].offset[1]){
                        bestMove[count].positions[0].x = pdfa->indexes[j].offset[0] * (-transformation2[direction].x) + point.x;
                        bestMove[count].positions[0].y = pdfa->indexes[j].offset[0] * (-transformation2[direction].y) + point.y;
                        bestMove[count].positions[1].x = pdfa->indexes[j].offset[1] * (-transformation2[direction].x) + point.x;
                        bestMove[count].positions[1].y = pdfa->indexes[j].offset[1] * (-transformation2[direction].y) + point.y;
                        if (m_board[bestMove[count].positions[0].x][bestMove[count].positions[0].y] != 0 ||
                            m_board[bestMove[count].positions[1].x][bestMove[count].positions[1].y] != 0)
                            //continue;
                            printf("It is wrong!!!!00000000000000000               %d\n",dfa_num);

                        if (check(bestMove, bestMove[count]))
                            count++;
                        }
                    else{
                        temp_bestMove.x = pdfa->indexes[j].offset[0] * (-transformation2[direction].x) + point.x;
                        temp_bestMove.y = pdfa->indexes[j].offset[0] * (-transformation2[direction].y) + point.y;
                        addpoint(bestMove, temp_bestMove);

                    }
                }
                /*��Ϊ���һ�����ӹ������У����ȼ�¼�¸�����*/
                else{
                    //Moves = (SMove *)realloc( bestMove, count * sizeof(Moves));
                    simple[sim_c].dfa = dfa_num;
                    simple[sim_c].x = pdfa->indexes[j].offset[0] * (-transformation2[direction].x) + point.x;
                    simple[sim_c].y = pdfa->indexes[j].offset[0] * (-transformation2[direction].y) + point.y;

                    for (int i=0; i<sim_c; i++){
                        if (simple[i].x == simple[sim_c].x && simple[i].y == simple[sim_c].y){
                            temp_bestMove.x = simple[sim_c].x;
                            temp_bestMove.y = simple[sim_c].y;
                            addpoint(bestMove, temp_bestMove);
                        }
                    }

                    if (m_board[simple[sim_c].x][simple[sim_c].y] != 0)
                        //continue;
                        printf("It is wrong!!!!1111111111111111111                %d\n",dfa_num);

                    else{
                        sim_c++;
                        temp.x = simple[sim_c-1].x;
                        temp.y = simple[sim_c-1].y;
                        m_board[temp.x][temp.y] = 2-my_color;
                        new_match(temp, bestMove, direction);
                        m_board[temp.x][temp.y] = 0;
                    }

                }
            }
            p = 0;
        }
        point.x += transformation2[direction].x;
        point.y += transformation2[direction].y;
        if (point.x > 19 || point.y > 19 || point.x < 1 || point.y < 1)
            break;
    }
}

/*ƥ�亯������ÿ����ʼ�㿪ʼ��һ��ֱ���ϵ����ӽ���ƥ��*/
void CDFA::match(pos_t point, int direction, move_t * bestMove)
{
    int i;
    pos_t temp;                /*��ʼ����ʱ����*/
                /*�з���ʱ����,���������з�����*/
    for (i=0; i<m_dfa_index; i++){
        temp = point;
        /*ƥ�����,����ȷ��״̬�Զ������ַ�������*/
        match2(temp, &m_dfa_array[i], bestMove, direction, i);
    }
    return;
}

/*����ƥ��������*/
int CDFA::pattern_match(char ourColor, move_t bestMove[])
{
    int i, j,  temp_count;
    pos_t point;

    char temp_board[GRID_NUM][GRID_NUM];

    my_color = 0;
    if ( ourColor == 1 )
        my_color = 1;
    copy_board(m_board, temp_board);

    /*�������ĸ��߽��72�����ӿ�ʼ�����̵�ÿ�����߽���ƥ��*/
    for (i=1; i<=19; i++){
        /*�ϱ߽�*/
        point.x = 1;
        point.y = i;
        match(point, u_2_d, bestMove);
        match(point, lu_2_rd, bestMove);
        match(point, ru_2_ld, bestMove);

        /*��߽�*/
        point.x = i;
        point.y = 1;
        match(point, l_2_r, bestMove);
        match(point, lu_2_rd, bestMove);
        match(point, ld_2_ru, bestMove);

        /*�±߽�*/
        point.x = 19;
        point.y = i;
        match(point, d_2_u, bestMove);
        match(point, ld_2_ru, bestMove);
        match(point, rd_2_lu, bestMove);

        /*�ұ߽�*/
        point.x = i;
        point.y = 19;
        match(point, r_2_l, bestMove);
        match(point, ru_2_ld, bestMove);
        match(point, rd_2_lu, bestMove);
    }
    /*��Ϊ˫�����������ģʽ�������з�*/
    for (j=0; j<sim_c; j++){
        for (i=j+1; i<sim_c; i++){
            bestMove[count].positions[0].x = simple[j].x;
            bestMove[count].positions[0].y = simple[j].y;
            bestMove[count].positions[1].x = simple[i].x;
            bestMove[count].positions[1].y = simple[i].y;
            if( (bestMove[count].positions[0].x != bestMove[count].positions[1].x ||
                 bestMove[count].positions[0].y != bestMove[count].positions[1].y) &&
                (m_board[bestMove[count].positions[0].x][bestMove[count].positions[0].y] == 0 &&
                 m_board[bestMove[count].positions[1].x][bestMove[count].positions[1].y] == 0)
               ){
                if (check(bestMove, bestMove[count]))
                    count++;
            }
        }
    }
    //kill_dfa();
    temp_count = count;
    count = 0;
    sim_c = 0;
    copy_board(temp_board, m_board);
    return temp_count;
}

//ԭ�� create_dfa.cpp �е�����


/*�ַ�Ԥ���庯��,��ģʽ����Ӧ�ַ�ת�������̵ı�ʾ����*/
int CDFA::find(char temp)
{
    if ( strchr("*", temp) )
        return 0;
    if ( strchr("X", temp) )
        return 1;
    if ( strchr("O", temp) )
        return 2;
    if ( strchr("$#+-|", temp) )
        return 3;
    return 0;
}

/*�Զ�����������,��ÿ��ģʽ��ö�ٵķ��������Զ���,��������״̬*/
/* For example:
 * create_dfa(pdfa, "OOX.")
 * gives:
 *
 *           2               2               1              0
 * (1,{}) -------> (2,{}) -------> (3,{}) -------> (4,{}) ------> (5,{2210})
 */
bool CDFA::dfa_create(dfa_t *pdfa, char str[])
{
    int i, j, k, l, temp, new_state=0, str_num;
    int x, y, num, mode;

    /*ģʽ�ַ�����Ϊ�Զ�����*/
    strcpy(pdfa->name, str);

    /*����ģʽ�ַ���,����״̬*/
    for (i=0; str[i]!='\0' && strchr("$#+-|OoXx.?,!a*", *str); i++){
        memset(pdfa->states[new_state].next, 0, 4 * sizeof(int));
        str_num = find(str[i]);
        pdfa->states[new_state].next[str_num] = new_state + 1;
        pdfa->states[new_state+1].att = str_num;

        /*��ÿ�ֲ����ϼ����ƽ�״̬�����������ת,���ӵ�ǰ����״̬*/
        for (l=0; l<4; l++){
            if (l == str_num)
                continue;
            for (j=new_state; j>=0; j--){

                /*������һ��δƥ���״̬*/
                if (pdfa->states[j].att == l){
                    temp = j;
                    /*�������Զ�����ʼ��,��ƥ������״̬��ת*/
                    for (k=1; pdfa->states[j-k].att == pdfa->states[new_state-k+1].att && j-k>=0; k++){
                        if (pdfa->states[j-k].att == str_num)
                            temp = j-k;
                    }
                    if (j-k<1){
                        pdfa->states[new_state].next[l] = j;
                        break;
                    }
                    if (temp < j)
                        j = temp+1;
                }
            }
        }
        new_state++;

        /*��״̬����ʱ����̬����ռ�*/
        if (new_state >= pdfa->max_states)
            dfa_resize(pdfa, pdfa->max_states + Size, pdfa->max_indexes + Size);
    }
    pdfa->last_state = new_state;

    /*�����з�,�������ڸ�ģʽ���з�������*/
    fscanf(patin,"%d",&num);
    for (i=0; i<num; i++){
        fscanf(patin,"%d",&mode);
        pdfa->indexes[i].mode = mode;
        /*��һ�ӵ�ģʽ*/
        if (mode == 1){
            fscanf(patin,"%d",&x);
            pdfa->indexes[i].offset[0] = x;
        }
        /*�¶��ӵ�ģʽ*/
        else{
            fscanf(patin,"%d %d",&x, &y);
            for ( ; mode>0; mode--){
                pdfa->indexes[i].offset[0] = x;
                pdfa->indexes[i].offset[1] = y;
            }
        }
    }
    pdfa->last_index = num;

    return 1;
}

/*�Զ��������ռ亯��,��״̬������з�����ʱ,���Զ�̬�����ռ���״̬���з�*/
void CDFA::dfa_resize(dfa_t *pdfa, int max_states, int max_indexes)
{
  state_t *pBuf;
  attrib_t *pBuf2;
  int i;

  pBuf = (state_t *)realloc(pdfa->states, max_states * sizeof(*pBuf));
  pBuf2 = (attrib_t *)realloc(pdfa->indexes, max_indexes * sizeof(*pBuf2));

  for (i = pdfa->max_states; i < max_states; i++)
    memset(pBuf + i, 0, sizeof(state_t));
  for (i = pdfa->max_indexes; i < max_indexes; i++)
    memset(pBuf2 + i, 0, sizeof(attrib_t));

  pdfa->states = pBuf;
  pdfa->max_states = max_states;
  pdfa->indexes = pBuf2;
  pdfa->max_indexes = max_indexes;
}

/*�Զ�����ʼ������,��patterns.in�ļ��ж�������ģʽ,���Զ�����ʼ��
 *��һ��Ϊģʽ����
 *ģʽ�����з�����, "ģʽ(char)" "�з���(int)" "����(int)  �з�ƫ����(int���Ӻ���ǰ����0��ʼ)"
 */

bool CDFA::dfa_init()
{
    char input[20];

    char path[500];

    memset(path,0,sizeof(path));
    int len=strlen(path);
    for (int i=len-1;i>=0;i--)
    {
        if (path[i]=='\\')
        {
            path[i+1]=0;
            break;
        }
    }

    strcat(path,"patterns.in");

    patin = fopen(path,"r");
    if (patin==NULL)
    {
        printf("Error: Can't open patterns.in for dfa!");
        return false;
    }
    int max_index;
    fscanf(patin,"%d",&max_index);
    m_dfa_index = 0;
    /*��������ģʽ*/
    for (int i =0; i < max_index; i++){
        fscanf(patin,"%s",input);
        memset(&m_dfa_array[m_dfa_index], 0, sizeof(m_dfa_array[0]));

        /*���� resize_dfa ������ģʽ,�з�����ռ�*/
        dfa_resize(&m_dfa_array[m_dfa_index], Size, Size);

        /*���� create_dfa ���������Զ���*/
        if ( dfa_create(&m_dfa_array[m_dfa_index], input) )
            m_dfa_index++;
    }
    fclose(patin);
    return true;
}

//ԭ�� kill_dfa.cpp �ļ��е�����

/*�������Զ�������ռ�����ͷ�*/
void CDFA::dfa_kill()
{
    int i;
    for (i=0; i<m_dfa_index; i++){
        free(m_dfa_array[i].states);
        free(m_dfa_array[i].indexes);
        memset(&m_dfa_array[i], 0, sizeof(m_dfa_array[i]));
    }
}

