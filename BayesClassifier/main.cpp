#include "Header.h"
/*
 1.读取所有数据，把有click的找出来
 2.在这87个里面，统计各个维度出现的频率
    例如，对于OS，P(OS_1|click)=(OS_1的个数 ／ 87)
 3.在全部里面，统计各个维度出现的频率
 4.click常数 ＝ 0.0670488
 */
#define CLICK_NUM 4346948
#define CLICK 0.0670488

Selector selector;

class basic_tasks
{
    HashTable hashtable;
    
    int indicator;
    Info *table;//Table of all Info data
    const int scale = 9615384;//Number of all records
    vector<int> clicked;
    
    //Predict the potiential of request_id clicking ads in request_pos
    void predict(HASH_VALUE request_id, HASH_VALUE request_pos)
    {
        //选定用户,根据用户生成用户向量
        //我偷懒，就选第一条，但是这个是一个可以做大量工作的地方
        Info selected_vec;
        for (int i=0; i<scale; i++)
        {
            if (table[i].id == request_id)
            {
                selected_vec = table[i];
            }
        }
        //选定监测点
        selected_vec.pos_id = request_pos;
        //生成完整向量
        //读入所有生成的概率表
        string forms[]={
            "cl_id","cl_pos","cl_ads","cl_ip","cl"
        };
        typedef HASH_VALUE* pHash_Value;
        pHash_Value requests[4]={
            &selected_vec.id,&selected_vec.pos_id,&selected_vec.ads_id,&selected_vec.ip_id
        };
        double P[4]={0};
        for (int i=0; i<4; i++)
        {
            fstream fin(forms[i]+".txt");
            string hashid;
            double potiential;
            while (fin>>hashid>>potiential)
            {
                if (HASH_VALUE(hashid) == *(requests[i]))
                {
                    P[i] = potiential;
                    fin.close();
                    break;
                }
            }
            if(P[i]==0) P[i]=1e-7;//A trick
            fin.close();
        }
        
        string stats[]={
            "id_stat","ip_stat","ads_stat","pos_stat"
        };
        double Q[4]={0};
        for (int i=0; i<4; i++)
        {
            fstream fin(stats[i]+".txt");
            string hashid;
            double potiential;
            while (fin>>hashid>>potiential)
            {
                if (HASH_VALUE(hashid) == *(requests[i]))
                {
                    Q[i] = potiential;
                    fin.close();
                    break;
                }
            }
            if(Q[i]==0) Q[i]=1e-7;//A trick
            fin.close();
        }
        //计算概率
        double final_p = CLICK * P[0]/Q[0] * P[1]/Q[1] * P[2]/Q[2] * P[3]/Q[3];
        printf("%.4lf\n",final_p);
    }
    int build_hash()
    {
        /* ip 的统计 */
        indicator = 0;
        for (int i=0; i<FULL_SCALE; i++, indicator++)
        {
            Info tmp = selector.sequence_read(i);
            char cache[20];
            memset(cache, 0, sizeof(cache));
            /* 下面这行是要修改的参数A */
            memcpy(cache, tmp.OS_info, 17);//拷贝字符串
            hashtable.insert(cache);
            //        int pos_in_hash = hashtable.find(cache);
            //        memcpy(hashtable[pos_in_hash].str, cache, 20);//插入源字符串
        }
        //写入文件,要修改的参数B
        fstream hf("os_hash.bin",ios::out|ios::binary);
        hf.write((char *)&hashtable, sizeof(hashtable));
        hf.close();
        return 0;
    }
    int build_hash_v2()
    {
        /* ip 的统计 */
        indicator = 0;
        for (int i=0; i<FULL_SCALE; i++, indicator++)
        {
            Info tmp = selector.sequence_read(i);
            //参数A
            hashtable.insert(tmp.OS_info);
            //        int pos_in_hash = hashtable.find(cache);
            //        memcpy(hashtable[pos_in_hash].str, cache, 20);//插入源字符串
        }
        //写入文件,要修改的参数B
        fstream hf("os_info_test.bin",ios::out|ios::binary);
        hf.write((char *)&hashtable, sizeof(hashtable));
        hf.close();
        return 0;
    }
    void check_all_clicked()
    {
        vector<Info> seq;
        int cnt = 0;
        for (int i=0; i<FULL_SCALE; i++)
        {
            Info tmp = selector.sequence_read(i);
            if (tmp.click)
            {
                //            printf("[%d] ID:%s Stable:%d OS:%s ACTION:%s \n",i,tmp.id.c_str(),
                //                   tmp.stable,tmp.OS_info,tmp.ads_id.c_str());
                cnt++;
                printf("%d\n",i);
            }
        }
        printf("Total %d",cnt);
    }
    
    HashTable t_id, t_ads, t_os, t_observer;
    void register_all_clicked()
    {
        int cnt_stable=0;
        int num;
        scanf("%d",&num);
        fstream fid("id_clicked.bin", ios::out|ios::binary);
        fstream fads("ads_clicked.bin", ios::out|ios::binary);
        fstream fos("os_clicked.bin", ios::out|ios::binary);
        fstream fobserver("observer_clicked.bin", ios::out|ios::binary);
        for(int i=0; i<num; i++)
        {
            int where;
            scanf("%d", &where);
            Info tmp = selector.sequence_read(where);
            char c1[20],c2[20],c3[20],c4[20];
            memset(c1, 0, sizeof(c1));
            memset(c2, 0, sizeof(c2));
            memset(c3, 0, sizeof(c3));
            memset(c4, 0, sizeof(c4));
            memcpy(c1, tmp.id.c_str(), 16);
            memcpy(c2, tmp.ads_id.c_str(), 16);
            memcpy(c3, tmp.OS_info, 17);
            memcpy(c4, tmp.pos_id.c_str(), 16);
            t_id.insert(c1);
            t_ads.insert(c2);
            t_os.insert(c3);
            t_observer.insert(c4);
            if (tmp.stable)
            {
                cnt_stable++;
            }
        }
        fid.write((char *)&t_id, sizeof(t_id)); fid.close();
        fads.write((char *)&t_ads, sizeof(t_ads)); fads.close();
        fos.write((char *)&t_os, sizeof(t_os)); fos.close();
        fobserver.write((char *)&t_observer, sizeof(t_observer)); fobserver.close();
        printf("%d\n",cnt_stable);
    }
    void register_os()
    {
        int cnt_stable=0;
        int num;
        scanf("%d",&num);
        fstream fos("os_clicked.bin", ios::out|ios::binary);
        for(int i=0; i<num; i++)
        {
            int where;
            scanf("%d", &where);
            Info tmp = selector.sequence_read(where);
            t_os.insert(tmp.OS_info);
            if (tmp.stable)
            {
                cnt_stable++;
            }
        }
        fos.write((char *)&t_os, sizeof(t_os)); fos.close();
        printf("%d\n",cnt_stable);
    }
    void count_all_stable()
    {
        int cnt=0;
        printf("total %d\n",FULL_SCALE);
        for (int i=0; i<FULL_SCALE; i++)
        {
            Info tmp = selector.sequence_read(i);
            if (tmp.stable)
            {
                cnt++;
            }
        }
        printf("cnt %d\n",cnt);
    }
    void count_all_clicked()
    {
        int cnt=0;
        for (int i=0; i<FULL_SCALE; i++)
        {
            Info tmp = selector.sequence_read(i);
            if (tmp.click)
            {
                cnt++;
            }
        }
        printf("clicked : %d, Totla : %d\n",cnt,FULL_SCALE);
    }
    
    
    //对于抽查的用户查看他出现的位置是否具有locality
    void check_id_locality()
    {
        vector<int> equs;
        Info chosen = selector.sequence_read(1281773);//抽查
        char cmp_buf[20]={0};
        memcpy(cmp_buf, chosen.id.c_str(), 16);
        for (int i=0; i<FULL_SCALE; i++)
        {
            Info tmp = selector.sequence_read(i);
            char buff[20]={0};
            memcpy(buff, tmp.id.c_str(), 16);
            if (memcmp(cmp_buf, buff, 16)==0)
            {
                equs.push_back(i);
            }
        }
        for (int i=0; i<equs.size(); i++ ) {
            printf("%d ",equs[i]);
        }
    }
    /* yes id has locality */
};

void load_hashtable(HashTable *table, fstream &fs)
{
    fs.read((char *)table, sizeof(HashTable));
}
HashTable *a_id, *a_ip, *a_pos, *a_os, *a_ads;
HashTable *c_id, *c_ip, *c_pos, *c_os, *c_ads;


class Tests
{
    HashTable hashtable;
    Info full_info[1300000];
    Info picked_records[1300000];
    void test3()
    {
        FILE *out = fopen("/Volumes/Hyakuya/test2.log", "w");
        int bias = FULL_SCALE - CLICK_NUM;
        
        double funda_huge = (double)bias;
        double funda_small = (double)CLICK_NUM;
        HashTable &t = *new HashTable;//新建一个hashtable用来判重
        HashTable &_tmin = *new HashTable;
        int last_step = 0;
        for (int iter=0; iter<(FULL_SCALE); iter++)
        {
            int vector_cnt = 0;
            /* 首先选择一个用户进行测试 */
            int user_num = iter;
            /* 补全这个用户的信息 需要找到这个用户的所有信息 */
            {
                Info chosen = selector.sequence_read(user_num);//抽查
                ///可能的瓶颈1
                char cmp_buf[20]={0};
                memcpy(cmp_buf, chosen.id.c_str(), 16);
                
                if (hashtable.find(cmp_buf))continue;
                
                hashtable.insert(cmp_buf);
                
                ///可以更新iter
                for (int i=iter; i<iter+1300000; i++)
                {
                    Info tmp = selector.sequence_read(i);
                    char buff[20]={0};
                    memcpy(buff, tmp.id.c_str(), 16);
                    if (memcmp(cmp_buf, buff, 16)==0)
                    {
                        full_info[vector_cnt]=tmp;
                        vector_cnt++;
                    }
                    else break;//because of locality
                }
            }
            
            int step;
            if ((step = iter / 100000) > last_step)
            {
                last_step = step;
                printf("iter %d\n",iter);
            }
            /* 我们现在整合这些信息 */
            /* 我们现在已经选定了用户了，我们还要选定pos */
            /* 观察数据可知pos不是很高的排序优先级，需要遍历整个vector */
            {
                ///!!!!!!!!!!
                _tmin.clear();
                
                for (int i=0; i<vector_cnt; i++)
                {
                    int picked_cnt = 1;
                    char buf[20]={0};
                    Info &sel = full_info[i];
                    
                    memcpy(buf, sel.pos_id.c_str(), 16);
                    Info *sel_gen;
                    ///重复用了t
                    if (_tmin.find(buf)==0)
                    {
                        picked_records[0]=full_info[i];
                        _tmin.insert(buf);
                        //再剩下的里面找出所有的这个条目
                        for (int j=i+1; j<vector_cnt; j++)
                        {
                            Info &crt = full_info[j];
                            if (memcmp(crt.pos_id.c_str(), buf, 16) == 0)
                            {
                                picked_records[picked_cnt]=crt;
                                picked_cnt++;
                            }
                        }
                        
                        /* 现在要整合出来一条，固定了pos和id的，天哪搞这个头都大，随手选算了 */
                        srand(*(int *)&buf[2] + (int)time(NULL));
                        int pick = rand()%picked_cnt;
                        sel_gen = &picked_records[pick];
                    }
                    else continue;
                    Info &gen = *sel_gen;
                    //搞到了gen ，开始bayes
                    double positive = 0.0, negtive = 0.0;
                    char b1[20],b2[20],b3[20],b4[20];
                    memcpy(b1, gen.ads_id.c_str(), 16);
                    memcpy(b2, gen.pos_id.c_str(), 16);
                    memcpy(b3, gen.OS_info, 17);
                    memcpy(b4, gen.ip_id.c_str(), 16);
                    b1[17]=0; b2[17]=0; b3[18]=0; b4[17]=0;
                    positive = (*a_ads)[a_ads->find(b1)].cnt/funda_huge *
                    (*a_pos)[a_pos->find(b2)].cnt/funda_huge *
                    (*a_os)[a_os->find(b3)].cnt/funda_huge *
                    funda_huge/FULL_SCALE;
                    
                    negtive = (*c_ads)[c_ads->find(b1)].cnt/funda_small *
                    (*c_pos)[c_pos->find(b2)].cnt/funda_small *
                    (*c_os)[c_os->find(b3)].cnt/funda_small *
                    funda_small / FULL_SCALE;
                    
                    if (positive < negtive)
                    {
                        char id_buf[20];
                        memcpy(id_buf, gen.id.c_str(), 16);
                        buf[17]=0;
                        fprintf(out,"%s\t%s\t%.20lf\t%.20lf\n",id_buf,b2,positive,negtive);
                    }
                }
            }
        }
        fclose(out);
        
        delete &t;
        delete &_tmin;
    }
};
class Test4
{
    Info full_info[1300000];
    Info picked_records[1300000];
    void test4()
    {
        FILE *out = fopen("/Volumes/Hyakuya/test2.log", "w");
        double bias = FULL_SCALE - CLICK_NUM;
        double funda_huge = (double)bias,  funda_small = (double)CLICK_NUM;
        HashTable &t = *new HashTable;//新建一个hashtable用来判重
        HashTable &_tmin = *new HashTable;
        HashTable &hashtable = *new HashTable;
        
        int last_step = 0;
        
        time_t last = time(0);
        for (int iter=0; iter<(FULL_SCALE); iter++)
        {
            int vector_cnt = 0;
            /* 首先选择一个用户进行测试 */
            int user_num = iter;
            /* 补全这个用户的信息 需要找到这个用户的所有信息 */
            {
                Info &chosen = selector.sequence_read(user_num, 0);
                if (hashtable.find(chosen.id.c_str()))continue;
                hashtable.insert(chosen.id.c_str());
                
                ///可以更新iter
                for (int i=iter; i<iter+1300000; i++)
                {
                    Info &tmp = selector.sequence_read(i, 0);
                    if (chosen.id==tmp.id)
                    {
                        full_info[vector_cnt]=tmp;
                        vector_cnt++;
                    }
                    else break;//because of locality
                }
            }
            
            int step;
            if ((step = iter / 1000000) > last_step)
            {
                time_t now = time(0);
                
                last_step = step;
                printf("iter %d interval %ld\n",iter,now-last);
                last=now;
                fflush(out);
            }
            /* 我们现在整合这些信息 */
            /* 我们现在已经选定了用户了，我们还要选定pos */
            /* 观察数据可知pos不是很高的排序优先级，需要遍历整个vector */
            {
                _tmin.clear();
                
                for (int i=0; i<vector_cnt; i++)
                {
                    int picked_cnt = 1;
                    Info &sel = full_info[i];
                    
                    Info *sel_gen;
                    ///重复用了t
                    if (_tmin.find(sel.pos_id.c_str())==0)
                    {
                        picked_records[0]=full_info[i];
                        _tmin.insert(sel.pos_id.c_str());
                        //再剩下的里面找出所有的这个条目
                        for (int j=i+1; j<vector_cnt; j++)
                        {
                            Info &crt = full_info[j];
                            if (crt.pos_id==sel.pos_id)
                            {
                                picked_records[picked_cnt]=crt;
                                picked_cnt++;
                            }
                            else break;
                        }
                        
                        /* 现在要整合出来一条，固定了pos和id的，天哪搞这个头都大，随手选算了 */
                        srand(sel.pos_id.c_str()[2] + (int)time(NULL));
                        int pick = rand()%picked_cnt;
                        sel_gen = &picked_records[pick];
                    }
                    else continue;
                    Info &gen = *sel_gen;
                    //搞到了gen ，开始bayes
                    double positive = 0.0, negtive = 0.0;
                    positive = (*a_ads)[a_ads->find(gen.ads_id.c_str())].cnt/funda_huge *
                    (*a_pos)[a_pos->find(gen.pos_id.c_str())].cnt/funda_huge *
                    (*a_os)[a_os->find(gen.OS_info)].cnt/funda_huge *
                    funda_huge/FULL_SCALE;
                    
                    negtive = (*c_ads)[c_ads->find(gen.ads_id.c_str())].cnt/funda_small *
                    (*c_pos)[c_pos->find(gen.pos_id.c_str())].cnt/funda_small *
                    (*c_os)[c_os->find(gen.OS_info)].cnt/funda_small *
                    funda_small / FULL_SCALE;
                    
                    if (positive < negtive)
                    {
                        fwrite(gen.id.c_str(), sizeof(char), 16, out);
                        fputc('\t', out);
                        fwrite(gen.pos_id.c_str(), sizeof(char), 16, out);
                        fputc('\n', out);
                        
                        //fprintf(out,"\t%.20lf\t%.20lf\n",positive,negtive);
                    }
                }
            }
        }
        fclose(out);
        
        delete &t;
        delete &_tmin;
    }
    
};

Info *full_info[1300000];
Info *picked_records[1300000];

void test5()
{
    FILE *out = fopen("/Volumes/Hyakuya/test2.log", "w");
    double bias = FULL_SCALE - CLICK_NUM;
    double funda_huge = (double)bias,  funda_small = (double)CLICK_NUM;
    HashTable &t = *new HashTable;//新建一个hashtable用来判重
    HashTable &_tmin = *new HashTable;
    HashTable &hashtable = *new HashTable;
    
    int last_step = 0;
    
    time_t last = time(0);
    for (int iter=0; iter<(FULL_SCALE); iter++)
    {
        int vector_cnt = 0;
        /* 首先选择一个用户进行测试 */
        int user_num = iter;
        /* 补全这个用户的信息 需要找到这个用户的所有信息 */
        {
            Info &chosen = selector.sequence_read(user_num, 0);
            if (hashtable.find(chosen.id.c_str()))continue;
            hashtable.insert(chosen.id.c_str());
            
            ///可以更新iter
            while (true)
            {
                Info &tmp = selector.sequence_read(iter, 0);
                if (chosen.id==tmp.id)
                {
                    full_info[vector_cnt]=&tmp;
                    vector_cnt++;
                    iter++;
                }
                else break;//because of locality
            }
        }
        
        int step;
        if ((step = iter / 100000) > last_step)
        {
            time_t now = time(0);
            
            last_step = step;
            printf("iter %d interval %ld\n",iter,now-last);
            last=now;
            fflush(out);
        }
        /* 我们现在整合这些信息 */
        /* 我们现在已经选定了用户了，我们还要选定pos */
        /* 观察数据可知pos不是很高的排序优先级，需要遍历整个vector */
        {
            _tmin.clear();
            
            for (int i=0; i<vector_cnt; i++)
            {
                int picked_cnt = 1;
                Info &sel = *full_info[i];
                
                Info *sel_gen;
                ///重复用了t
                if (_tmin.find(sel.pos_id.c_str())==0)
                {
                    picked_records[0]=full_info[i];
                    _tmin.insert(sel.pos_id.c_str());
                    //再剩下的里面找出所有的这个条目
                    for (int j=i+1; j<vector_cnt; j++)
                    {
                        Info &crt = *full_info[j];
                        if (crt.pos_id==sel.pos_id)
                        {
                            picked_records[picked_cnt]=&crt;
                            picked_cnt++;
                        }
                        else break;
                    }
                    
                    /* 现在要整合出来一条，固定了pos和id的，天哪搞这个头都大，随手选算了 */
                    srand(sel.pos_id.c_str()[2] + (int)time(NULL));
                    int pick = rand()%picked_cnt;
                    sel_gen = picked_records[pick];
                }
                else continue;
                Info &gen = *sel_gen;
                //搞到了gen ，开始bayes
                double positive = 0.0, negtive = 0.0;
                positive = (*a_ads)[a_ads->find(gen.ads_id.c_str())].cnt/funda_huge *
                (*a_pos)[a_pos->find(gen.pos_id.c_str())].cnt/funda_huge *
                (*a_os)[a_os->find(gen.OS_info)].cnt/funda_huge *
                funda_huge/FULL_SCALE;
                
                negtive = (*c_ads)[c_ads->find(gen.ads_id.c_str())].cnt/funda_small *
                (*c_pos)[c_pos->find(gen.pos_id.c_str())].cnt/funda_small *
                (*c_os)[c_os->find(gen.OS_info)].cnt/funda_small *
                funda_small / FULL_SCALE;
                
                if (positive < negtive)
                {
                    fwrite(gen.id.c_str(), sizeof(char), 16, out);
                    fputc('\t', out);
                    fwrite(gen.pos_id.c_str(), sizeof(char), 16, out);
                    fputc('\n', out);
                    
                    //fprintf(out,"\t%.20lf\t%.20lf\n",positive,negtive);
                }
            }
        }
    }
    fclose(out);
    
    delete &t;
    delete &_tmin;
}


void ptest1()
{
    fstream faid("id_hash.bin",ios::in|ios::binary);
    fstream faads("ads_hash.bin",ios::in|ios::binary);
    fstream fapos("pos_hash.bin",ios::in|ios::binary);
    fstream faos("os_hash.bin",ios::in|ios::binary);
    
    fstream fcid("id_clicked.bin",ios::in|ios::binary);
    fstream fcads("ads_clicked.bin",ios::in|ios::binary);
    fstream fcos("os_clicked.bin",ios::in|ios::binary);
    fstream fcpos("observer_clicked.bin",ios::in|ios::binary);
    
    a_id = new HashTable, a_pos = new HashTable;
    a_os = new HashTable, a_ads = new HashTable;
    c_id = new HashTable, c_pos = new HashTable;
    c_os = new HashTable, c_ads = new HashTable;
    
    load_hashtable(a_id, faid);
    load_hashtable(a_pos, fapos);
    load_hashtable(a_os, faos);
    load_hashtable(a_ads, faads);
    
    printf("half loaded\n");
    
    load_hashtable(c_id, fcid);
    load_hashtable(c_pos, fcpos);
    load_hashtable(c_os, fcos);
    load_hashtable(c_ads, fcads);
    
    printf("start testing\n");
    test5();
    
    delete a_id , delete a_pos ;
    delete a_os , delete a_ads ;
    delete c_id , delete c_pos ;
    delete c_os , delete c_ads ;
}
int main()
{
    ptest1();
    //register_os();
    return 0;
}





