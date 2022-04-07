Idiom: Early Return.

1.Whole-scope conditional, into early return/early continue.

This is particularly good if you have nested whole-scope conditionals

ie

```c++
void do_stuff(int data[], bool flag_1, bool flag_2){
    if (flag_1){
        munge_data(data);
        if(flag_2){
            munge_data_again(data);
        } else {
        printf("flag_2 was false");
        };
    } else {
    printf("flag_1 was false");
    };
    return;
}
```

```c++
void do_stuff_2(int data[], bool flag_1, bool flag_2){
    if (!flag_1){
        printf("flag_1 was false");
        return;
    };
    munge_data(data);
    if(!flag_2){
        printf("flag_2 was false");
        return;
    };
    munge_data_again(data);
    return;
}
```
