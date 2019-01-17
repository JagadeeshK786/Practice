#include <iostream>
#include <initializer_list>
using namespace std;

//Definition for singly-linked list.
struct ListNode
{
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(0) {}
};

int doCarry(int val0, bool &carry)
{
    if (val0 > 9)
    {
        carry=true;
        return val0 - 10;
    }
    else
    {
        carry = false;
        return  val0;
    }
}

ListNode *makeList(const std::initializer_list<int> &values)
{
    if (values.begin() == values.end())
        return nullptr;
    ListNode *head = new ListNode(*values.begin());
    ListNode *current = head;
    for (auto run = values.begin() + 1; run != values.end(); run++)
    {
        current->next = new ListNode(*run);
        current = current->next;
    }
    return head;
}

ListNode *addTwoNumbers(ListNode *l1, ListNode *l2)
{
    ListNode zeroObject{0};
    
    auto moveNext = [](ListNode *&iter) {
        if (iter!=nullptr&&iter->next)
        {
            iter = iter->next;
            return true;
        }
        return false;
    };
    //    auto hasNext=[](ListNode* iter)->bool{return iter->next;};
    
    ListNode * result=0;
    auto push_result=[&result,moveNext](int value){
        if(result==0)
        {
            result=new ListNode(value);
            return;
        }
        auto p=result;
        while(moveNext(p));
        p->next=new ListNode(value);
    };
    
    bool carry=false;
    while(true){
        auto roundResult=0;
        if(l1!=nullptr)
            roundResult+=l1->val;
        if(l2!=nullptr)
            roundResult+=l2->val;
        if(carry)
            roundResult++;
        push_result( doCarry(roundResult, carry));
        
        bool fuck=!moveNext(l1);
        if(fuck)
            l1=nullptr;
        bool sucks=!moveNext(l2);
        if(sucks)
            l2=nullptr;
        if(fuck&&sucks)
            break;
    }
    
    if(carry){
        push_result( 1);
    }
    
    return result;
}

int main()
{
    auto first = makeList({9,8}),second=makeList({1});
    auto result= addTwoNumbers(first, second);
    std::cout << "";
}



