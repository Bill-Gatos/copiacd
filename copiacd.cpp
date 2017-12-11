#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
int error(char *f)
{
	fprintf(stderr,"No se puede abrir %s\n",f);
	return 1;
}
int copiar(char *iso,int bloque)
{
	char disco[]="/dev/cdrom";
	int fd1,fd2;
	int n;
        long long nbytes;
        char bufer[bloque];
	
	if((fd1=open64(disco,O_RDONLY))==-1) return error(disco);
	if((fd2=open64(iso,O_WRONLY|O_CREAT|O_EXCL,0666))==-1 && errno==EEXIST)
	{
		printf("El archivo ya existe.\n\t1.Sobrescribir\n\t2.Continuar copia\n\t3.Cancelar\n");
		switch(getchar())
		{
			case '1':
				if((fd2=open64(iso,O_WRONLY|O_CREAT|O_TRUNC,0666))==-1) return error(iso);
				break;
			case '2':
				if((fd2=open64(iso,O_WRONLY|O_CREAT|O_APPEND,0666))==-1) return error(iso);
				break;
			case '3':
			default:
				return 1;
		}
	}
        nbytes=lseek64(fd2,0L,SEEK_END);
        lseek64(fd1,nbytes,SEEK_SET);
        while((n=read(fd1,bufer,bloque))!=0)
        {
                if(n>0)
                {
                        write(fd2,bufer,n);
                        nbytes+=n;
                        printf("\r%lld bytes copiados",nbytes);
                }
        }
	putchar('\n');
	close(fd1);
	close(fd2);
	return 0;
}
main(int argc,char *argv[])
{
	int i;
	int bloque=2048;
	if(argc<2)
	{
		printf("Sintaxis: copiacd fichero.iso [tamaño_bloque_bytes]\n");
		return 1;
	}
        else if(argc==3) bloque=atoi(argv[2]);
	if(copiar(argv[1],bloque)) return 1;
	else return 0;
}