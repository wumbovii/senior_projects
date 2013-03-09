%%

p : a '(' list1 ')'
  | b '(' list2 ')'
;

a : 'I' ;

b : 'I' | 'J' ;

list1 : /* EMPTY */ 
      | list1.1
      ;

list1.1 : 'x'
      | list1.1 ',' 'x'
      ;

list2 : 'y'
      | list2 ',' 'y'
      ;

%%


