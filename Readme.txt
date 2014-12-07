Compilation : 
   cc -I./. server.c patricia.c -o server

Random Number Generator : 
Usage :: random_number_gen.sh <Max Range> <path_to_file_to_store_random_numbers>

eg : sh random_number_gen.sh 1000 Random_Numbers.txt

To compile Server :
  gcc -Wall server_select.c patricia.c common.c -lpthread -o server

for compiling client code:
  gcc -std=gnu99 -o client client.c common.c -lpthread

