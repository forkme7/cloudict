/*
 * Copyright (c) 2008-2013 Hao Cui<>,
 *                         Liang Li<liliang010@gmail.com>,
 *                         Ruijian Wang<>,
 *                         Siran Lin<>.
 *                         All rights reserved.
 *
 * This program is a free software; you can redistribute it and/or modify
 * it under the terms of the BSD license. See LICENSE.txt for details.
 *
 * 2013/11/01
 *
 */

#ifndef VCFSEARCH_H_
#define VCFSEARCH_H_

#include <vector>

#include "pattern.h"

#define HASHSIZE 19997

//VCFSearch
typedef struct _node
{
    pos_t p1;
    pos_t p2;
    int pre, next;
}node_t;

typedef struct _listnode
{
    int pos;
    int score;
    int dist;
}ListNode;

typedef struct _hashnode
{
    int dep;
    unsigned long hash;
    int pre;
    move_t move;
    bool res;
    char color;
}HashNode;

class CVCFSearch {
    public:
        CVCFSearch();
        CVCFSearch(char* ptr_board[GRID_NUM], char* ptr_chess_type);

        //�ж��Ƿ���ҪVCF������
        bool vcf_judge(move_t * preMove);

        int init();

        void init_game();

        void before_search(char board[][GRID_NUM], char color);

        //VCF��ʵ�֣�bestMove:��������з���preMove:������һ���з�;
        bool vcf_search(int depth,char ourColor,move_t * bestMove,move_t * preMove, int preNode, int prePos);

        //����VCF��ʵ�֣�bestMove:��������з���preMove:������һ���з�;
        bool anti_vcf_search(int depth,char ourColor,move_t * bestMove,move_t * preMove, int preNode, int prePos);

        //��ȡVCF�з���a_d:��ǰ�ǽ������Ƿ��أ�canUse:�����ѡ��ĵ㣻moveList:�����з����к����г��ȣ�
        int vcf_get_move_list( char ourColor,char a_d, pos_t * canUse, int n_Pos, move_t * moveList, move_t* pretMove);

    private:
        //�ж��Ƿ������ourColor:��ǰ�����ɫ��preMove:������һ�������ļ�����
        //bool IsWin(char ourColor, SMove * preMove);

        //�жϸ��߷��Ƿ������в
        int is_attack(char board[][GRID_NUM],char Color, move_t * Move);

        //�жϸ��߷��Ƿ񹹳ɻ���
        int is_dlb_attack(char board[][GRID_NUM],char Color, move_t * Move);
        void vcf_make_move(char Color, move_t * move);            //����
        void vcf_unmake_move(char Color, move_t * move);        //�����з�
        int vcf_hash_check(HashNode node);
        unsigned long vcf_hash_board(char board[GRID_NUM][GRID_NUM]);

        int is_three(char position[GRID_NUM][GRID_NUM],char Color, pos_t * Pos);
        void sort(move_t * moveList, int n_moveList, move_t * preMove);
        inline int vcf_abs(int a);
        inline int dist(pos_t p1, pos_t p2, pos_t pt);
        inline int vcf_min(int a, int b);

    public:
        int                 m_vcf_node;

    private:
        CDFA                m_dfa;

        char                m_board[GRID_NUM][GRID_NUM];
        char                m_chess_type;
        char                m_has_win;

        char                m_org_board[GRID_NUM][GRID_NUM];
        ListNode            m_list_node[10000];
        move_t              m_tmp_move_list[10000];
        move_t              m_vcf_move_list[VCFDEPTH+1][10000];                        //���ɵ��з�����
        std::vector<int>    m_hash_map[VCFDEPTH+1][HASHSIZE];
        HashNode            m_hash_que[1000000];

        node_t              m_vcf_move_table[100000];
        int                 m_vcf_total_node;
        int                 m_vcf_now_pos;
        int                 m_dy[4];
        int                 m_dx[4];        //���巽������
        char                m_vcf_use[GRID_NUM][GRID_NUM][4];                    //���۱������
        char                m_vcf_mark[GRID_NUM][GRID_NUM];

};

#endif

