# Multithreaded Server Implementation:
## PT-BR:

<p>
Este trabalho foi relizado na disciplica de Sistemas Operacionais I na Universidade Federal do Maranhão , basicamente consiste na implementação de um servidor multithreads em C , na qual dada uma quantidade de requisições para processamento um servidor inicializa uma quantidade N de threads as quais realização cálculos acrca da requisição. O programa gera as M requisições no formato "i;tempo" , onde i=Número inteiro e "tempo"=tempo que o servidor tem que esperar após ter realizado os cálculos. O parâmetro "i" é a  quantidade de casas decimais a qual o programa irá calcular o valor de pi (exemplo: se i=2 então o valore retornado na requisição será 3.14). Por fim o servidor deverá criar um arquivo para cada uma das N threads criadas , onde cada arquivo desses armazena o valor de pi e quando foi calculado.
</p>

* Bibliotecas usadas:
> pthread
> string.h
> stdio.h
> stdlib.h
> time.h
> math.h
Caso não tenha, execute o seguinte comando:
sudo apt-get install libpthread-stubs0-dev

* Código para compilação:
1) Passo:
gcc main.c -o trabalho_thread -lm

2) Passo:
./trabalho_thread

* Execução online:
https://replit.com/@LuisMiguel27/Implementacao-Servidor-Multithread#main.c

## EN:
<p>
This work was carried out in the course of Operating Systems I at the Federal University of Maranhão, basically consists of the implementation of a multithreaded server in C, in which given a number of requests for processing, a server initializes an amount N of threads which carry out calculations about the request. The program generates the M requests in the "i;time" format, where i=Integer number and "time"=time the server has to wait after performing the calculations. The "i" parameter is the number of decimal places in which the program will calculate the value of pi (example: if i=2 then the value returned in the request will be 3.14). Finally, the server must create a file for each of the N threads created, where each file stores the value of pi and when it was calculated.
</p>

* Libraries used:
> pthread
> string.h
> stdio.h
> stdlib.h
> time.h
> math.h
If not, run the following command:
sudo apt-get install libpthread-stubs0-dev

* Code for compilation:
1) step:
gcc main.c -o work_thread -lm

2) Step:
./work_thread

* Online execution:
https://replit.com/@LuisMiguel27/Implementacao-Servidor-Multithread#main.c