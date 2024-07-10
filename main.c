#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include<math.h>
#include<stdbool.h>
//https://www.youtube.com/watch?v=kL3lKLdGlzY youtube linki
char colors[7][12]={
    "\033[0;37m",
    "\033[0;31m",//green
    "\033[0;32m",//yellow
    "\033[0;33m",
    "\033[0;34m",
    "\033[0;35m",
    "\033[0;36m",
    
};
typedef struct MoveStruct{
    struct MoveStruct *up,*down,*left,*right;
    int i,j,depth,maxDepth,score,enemyScore,enemyDepth;
}moveStruct;
moveStruct* findPaths(int** board,int n,int *colorPoints,int *enemyColorPoints,int i,int j);
void freeMoveStruct(moveStruct* move);
int findMin(int* list,int n){
    int min=INT_MAX,i;
    for(i=0;i<n;i++)
        if(list[i]<min)
            min=list[i];
        
    return min;
}
int findDigitNumber(int x){
    int i=1;
    x/=10;
    while(x>=1){
        x/=10;
        i++;
    }
    return i;
}
void printSpaces(int n){
    while(n--)printf(" ");
}
void drawBord(int** boardPtr,int n,int **colorPoints,int playerCount,int aicount,int* scores){
    printf("\e[H\e[2J\e[3J");
    int i=0,j=0,spaceCount=log10(n)+1;
    for(i=0;i<playerCount;i++){
        
        printf("%d. Player score=%d\n",i+1,scores[i]);
        for(j=0;j<5;j++){
            printf("%s%d ",colors[j+1],colorPoints[i][j]);
        }
        printf("\033[1;0m\n");

    }
    for(i=0;i<aicount;i++){
        
        printf("%d. Ai score=%d\n",i+1,scores[playerCount+i]);
        for(j=0;j<5;j++){
            printf("%s%d ",colors[j+1],colorPoints[playerCount+i][j]);
        }
        printf("\033[1;0m\n");

    }
    printf("\n");
    
    printSpaces(spaceCount+1);
    
    
    for(i=1;i <n+1;i++)
    {
        printf("%d",i-1);
        printSpaces(spaceCount-findDigitNumber(i)+1);
    }
    printf("\n");
    for(i =0; i<n;i++){
        printf("%d",i);
        printSpaces(spaceCount-findDigitNumber(i)+1);
        for(j =0;j<n;j++){
            printf("%s",colors[boardPtr[i][j]]);
            printf("%c",boardPtr[i][j]? boardPtr[i][j]+'A': ' ');
            printSpaces(spaceCount);
            printf("\033[1;0m");
        }
        printf("%d\n",i);
    }
    
    printSpaces(spaceCount+1);
    for(i=1 ;i <n+1;i++){
        printf("%d",i-1);
        printSpaces(spaceCount-findDigitNumber(i)+1);
    }
    printf("\n\n");
}
int findTheNumberOfMovesForAPiece(int **board,int n,int i,int j){
    int count=0;
    if(i<n&&j<n&&j>=0&&i>=0){
        if(board[i][j]!=0){
            if(i+2<n)if(board[i+1][j]!=0&&board[i+2][j]==0)count++;//check if it can jump down
            if(i-2>=0)if(board[i-1][j]!=0&&board[i-2][j]==0)count++;//check if it can jump up
            if(j+2<n)if(board[i][j+1]!=0&&board[i][j+2]==0)count++;//check if it can jump right
            if(j-2>=00)if(board[i][j-1]!=0&&board[i][j-2]==0)count++;//check if it can jump left
        }
    }
    return count;
}
void saveGame(int** board,int **colorPoints,int n,int playercount,int playerToPlayIndex,int aiCount,int aiIndex,int iStart, int jStart,char *fileName){
    FILE *fptr=fopen(fileName,"wb");
    int i,j;
    fwrite(&n,sizeof(int),1,fptr);
    fwrite(&playercount,sizeof(int),1,fptr);
    fwrite(&playerToPlayIndex,sizeof(int),1,fptr);
    fwrite(&aiCount,sizeof(int),1,fptr);
    fwrite(&aiIndex,sizeof(int),1,fptr);
    fwrite(&iStart,sizeof(int),1,fptr);
    fwrite(&jStart,sizeof(int),1,fptr);
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            fwrite(&board[i][j],sizeof(board[i][j]),1,fptr);
        }
    }
    for(i=0;i<playercount;i++){
        for(j=0;j<5;j++){
            fwrite(&colorPoints[i][j],sizeof(colorPoints[i][j]),1,fptr);
        }
    }
    fclose(fptr);
}
bool checkIfTheGameIsOver(int** board,int n){
    int i,j;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            if(board[i][j]!=0)
            {
                if(i+2<n)if(board[i+1][j]!=0&&board[i+2][j]==0)return 0;//check if it can jump down
                if(i-2>=0)if(board[i-1][j]!=0&&board[i-2][j]==0)return 0;//check if it can jump up
                if(j+2<n)if(board[i][j+1]!=0&&board[i][j+2]==0)return 0;//check if it can jump right
                if(j-2>=0)if(board[i][j-1]!=0&&board[i][j-2]==0)return 0;//check if it can jump left
            }
        }
    }
    return 1;
}
bool makeMove(int **board,int n ,int **colorPoints,int playerCount,int playerIndex,int iStart,int jStart,int iTarget,int jTarget){
    if((abs(iStart-iTarget)==2&&iTarget<n&&iTarget>=0&&jStart==jTarget)||(abs(jStart-jTarget)==2&&jTarget<n&&jTarget>=0&&iStart==iTarget)){//check if the move is moving 2 spaces and isnt out of range or diagonal
        if(board[(iStart+iTarget)/2][(jStart+jTarget)/2]!=0&&board[iTarget][jTarget]==0){//check if there is a piece to jump over and if the target place is empty
            colorPoints[playerIndex][board[(iStart+iTarget)/2][(jStart+jTarget)/2]-1]++;//add a point to player
            board[(iStart+iTarget)/2][(jStart+jTarget)/2]=0;//remove the jumped over chip
            board[iTarget][jTarget]=board[iStart][jStart];//copy the starting piece to target place 
            board[iStart][jStart]=0;//remove the starting piece
            
            return 1;
        }
    }
    return 0;
}
bool checkIfItsABetterMove(moveStruct *move,moveStruct *moveToCompareAgainst){//find if a move is better than the other one
    if(moveToCompareAgainst==NULL)return 1;//if the other move isnt set it is better
    if(move->score>moveToCompareAgainst->score||(move->score==moveToCompareAgainst->score&&move->enemyScore<moveToCompareAgainst->enemyScore)||(move->score==moveToCompareAgainst->score&&move->enemyScore==moveToCompareAgainst->enemyScore
    &&move->depth>moveToCompareAgainst->depth)||(move->score==moveToCompareAgainst->score&&move->enemyScore==moveToCompareAgainst->enemyScore&&move->depth==moveToCompareAgainst->depth&&move->enemyDepth<=moveToCompareAgainst->enemyDepth)){
            return 1;//prioritys score>enemyscore>depth>enemy depth
    }
        return 0;
}
moveStruct* selectPath(moveStruct* tempMove){
    int maxScore=0,maxDepth=0,minEnemyScore=INT_MAX,minEnemyDepth=INT_MAX;
    moveStruct* tempMoveNext=NULL;
    if(tempMove->up){
        if(checkIfItsABetterMove(tempMove->up,tempMoveNext))tempMoveNext=tempMove->up;
    }
    if(tempMove->down){
        if(checkIfItsABetterMove(tempMove->down,tempMoveNext))tempMoveNext=tempMove->down;
    }
    if(tempMove->right){
        if(checkIfItsABetterMove(tempMove->right,tempMoveNext))tempMoveNext=tempMove->right;
    }
    if(tempMove->left){
        if(checkIfItsABetterMove(tempMove->left,tempMoveNext))tempMoveNext=tempMove->left;
    }
    return tempMoveNext;
}
moveStruct* findPathsOnTheWholeBoard(int** board,int n,int* colorPoints,int* enemyColorPoints){
    int i,j;
    moveStruct* tempMove,*bestMove=calloc(1,sizeof(moveStruct));
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            tempMove=findPaths(board,n,colorPoints,enemyColorPoints,i,j);
            if(tempMove->score>bestMove->score||(tempMove->score==bestMove->score&&tempMove->depth>=bestMove->depth)){
                freeMoveStruct(bestMove);
                bestMove=tempMove;
            }
            else {
                freeMoveStruct(tempMove);
            }
        }
    }
    return bestMove;
}
moveStruct* findPaths(int** board,int n,int *colorPoints,int *enemyColorPoints,int i,int j){//i know that there are 5 colors so there isnt another variable for colorPoints size
    moveStruct* move=(moveStruct*)calloc(1,sizeof(moveStruct)),*enemyTemp;
    int k,l,tempChip;
    move->i=i;
    move->j=j;
    move->depth=0;
    move->maxDepth=0;
    move->score=findMin(colorPoints,5);
    if(enemyColorPoints!=NULL){
        enemyTemp= findPathsOnTheWholeBoard(board,n,enemyColorPoints,NULL);
        move->enemyScore=enemyTemp->score;
        move->enemyDepth=enemyTemp->depth;
    }
    if(i<n&&j<n&&j>=0&&i>=0){
        if(board[i][j]!=0){
            if(i+2<n)if(board[i+1][j]!=0&&board[i+2][j]==0)//check if it can jump down
            {
                tempChip=board[i+1][j];//change the temp board for recursion
                board[i+1][j]=0;
                board[i+2][j]=board[i][j];
                board[i][j]=0;
                colorPoints[tempChip-1]++;
                move->down=findPaths(board,n,colorPoints,enemyColorPoints,i+2,j);//check if it can jump down


                if(move->down->enemyScore<move->enemyScore){
                    move->enemyScore=move->down->enemyScore;
                    move->enemyDepth=move->down->enemyDepth;
                }
                else if(move->down->enemyScore==move->enemyScore){
                    if((move->down)->enemyDepth<move->enemyDepth){
                        move->enemyDepth=move->down->enemyDepth;
                    }
                }

                if(move->down->score>move->score){
                    move->score=move->down->score;
                    move->depth=move->down->depth+1;
                }
                else if(move->down->score==move->score){
                    if((move->down)->depth>=move->depth){
                        move->depth=move->down->depth+1;
                    }
                }
                if((move->down)->maxDepth>=move->maxDepth)move->maxDepth=(move->down)->maxDepth+1;
                board[i+1][j]=tempChip;//take the changes back so rest can work as well
                board[i][j]=board[i+2][j];
                board[i+2][j]=0;
                
                colorPoints[tempChip-1]--;
            }
            if(i-2>=0)if(board[i-1][j]!=0&&board[i-2][j]==0)//check if it can jump up
            {
                tempChip=board[i-1][j];//change the temp board for recursion
                board[i-1][j]=0;
                board[i-2][j]=board[i][j];
                board[i][j]=0;
                colorPoints[tempChip-1]++;
                move->up=findPaths(board,n,colorPoints,enemyColorPoints,i-2,j);//check if it can jump up
                
                if(move->up->enemyScore<move->enemyScore){
                    move->enemyScore=move->up->enemyScore;
                    move->enemyDepth=move->up->enemyDepth;
                }
                else if(move->up->enemyScore==move->enemyScore){
                    if((move->up)->enemyDepth<move->enemyDepth){
                        move->enemyDepth=move->up->enemyDepth;
                    }
                }

                if(move->up->score>move->score){
                    move->score=move->up->score;
                    move->depth=move->up->depth+1;
                }
                else if(move->up->score==move->score){
                    if((move->up)->depth>=move->depth){
                        move->depth=move->up->depth+1;
                    }
                }
                if((move->up)->maxDepth>=move->maxDepth)move->maxDepth=(move->up)->maxDepth+1;

                board[i-1][j]=tempChip;//take the changes back so rest can work as well
                board[i][j]=board[i-2][j];
                board[i-2][j]=0;
                colorPoints[tempChip-1]--;
            }
            if(j+2<n)if(board[i][j+1]!=0&&board[i][j+2]==0)//check if it can jump right
            {
                tempChip=board[i][j+1];//change the temp board for recursion
                board[i][j+1]=0;
                board[i][j+2]=board[i][j];
                board[i][j]=0;
                colorPoints[tempChip-1]++;
                move->right=findPaths(board,n,colorPoints,enemyColorPoints,i,j+2);//check if it can jump right

                if(move->right->enemyScore<move->enemyScore){
                    move->enemyScore=move->right->enemyScore;
                    move->enemyDepth=move->right->enemyDepth;
                }
                else if(move->right->enemyScore==move->enemyScore){
                    if((move->right)->enemyDepth<move->enemyDepth){
                        move->enemyDepth=move->right->enemyDepth;
                    }
                }

                if(move->right->score>move->score){
                    move->score=move->right->score;
                    move->depth=move->right->depth+1;
                }
                else if(move->right->score==move->score){
                    if((move->right)->depth>=move->depth){
                        move->depth=move->right->depth+1;
                    }
                }
                if((move->right)->maxDepth>=move->maxDepth)move->maxDepth=(move->right)->maxDepth+1;
                board[i][j+1]=tempChip;//take the changes back so rest can work as well
                board[i][j]=board[i][j+2];
                board[i][j+2]=0;
                colorPoints[tempChip-1]--;
            }
            if(j-2>=0)if(board[i][j-1]!=0&&board[i][j-2]==0)//check if it can jump left
            {
                tempChip=board[i][j-1];//change the temp board for recursion
                board[i][j-1]=0;
                board[i][j-2]=board[i][j];
                board[i][j]=0;
                colorPoints[tempChip-1]++;
                move->left=findPaths(board,n,colorPoints,enemyColorPoints,i,j-2);//check if it can jump left

                if(move->left->enemyScore<move->enemyScore){
                    move->enemyScore=move->left->enemyScore;
                    move->enemyDepth=move->left->enemyDepth;
                }
                else if(move->left->enemyScore==move->enemyScore){
                    if((move->left)->enemyDepth<move->enemyDepth){
                        move->enemyDepth=move->left->enemyDepth;
                    }
                }

                if(move->left->score>move->score){
                    move->score=move->left->score;
                    move->depth=move->left->depth+1;
                }
                else if(move->left->score==move->score){
                    if((move->left)->depth>=move->depth){
                        move->depth=move->left->depth+1;
                    }
                }
                if((move->left)->maxDepth>=move->maxDepth)move->maxDepth=(move->left)->maxDepth+1;
                board[i][j-1]=tempChip;//take the changes back so rest can work as well
                board[i][j]=board[i][j-2];
                board[i][j-2]=0;
                colorPoints[tempChip-1]--;
            }
            //calloc will set the unplayable moves to null anyways so i dont do it myself
        }
    }
    return move;
}
void freeMoveStruct(moveStruct* move){
    if(move->up)freeMoveStruct(move->up);
    if(move->down)freeMoveStruct(move->down);
    if(move->left)freeMoveStruct(move->left);
    if(move->right)freeMoveStruct(move->right);
    free(move);
}
int sumOfTheList(int* list,int n){
    int sum=0,i;
    for ( i = 0; i < n; i++)
    {
        sum+=list[i];
    }
    return sum;
}
void game(char* fileName){
    int n,**colorPoints,i,j,tempColorCount[5]={0,0,0,0,0},iTarget=-2,jTarget=-2,iStart=-2,jStart=-2,*scores,playerCount,playerIndex=0,tempChip,aiCount,aiIndex=0,maxRows=0,maxExtraSkippers=0,maxPointsIndex;
    char newFileName[100];
    bool playerIsDone=0;
    int **board;
    if(fileName==NULL){
        printf("Lutfen tahta boyunu girin\n");//ask for the board size
        scanf("%d",&n);
        while(n<5){
            printf("Tahta boyutu minimum 5 olabilir kalanlar ortadaki bosluktan oturu ya direk bitiyorlar ya da bos oluyorlar\n");
            scanf("%d",&n);
        }
        printf("Lutfen oyuncu sayisini girin\n");//ask for the board size
        scanf("%d",&playerCount);
        printf("Lutfen yapay zeka sayisini girin\n");//ask for the board size
        scanf("%d",&aiCount);
        board=(int**)calloc(n,sizeof(int*));
        colorPoints=(int**)calloc(playerCount+aiCount,sizeof(int*));
        scores=(int*)calloc(playerCount+aiCount,sizeof(int));
        for(i=0;i<playerCount+aiCount;i++)colorPoints[i]=(int*)calloc(5,sizeof(int));
        for(i=0;i<5;i++)tempColorCount[i]=n*n/5+1;//+1 since computer rounds down and there might be problems
        for(i=0;i<n;i++){
            board[i]=(int*)calloc(n,sizeof(int));//initialize the array
            for(j=0;j<n;j++){
                int ran=rand()%5;
                while(tempColorCount[ran]==0)ran=rand()%5;
                board[i][j]=ran+1;
                tempColorCount[ran]--;
            }
        }
        if(n%2==0){
            board[n/2 -1][n/2-1]=0;
            board[n/2 -1][n/2]=0;
            board[n/2][n/2-1]=0;
            board[n/2][n/2]=0;
        }
        else{
            board[n/2][n/2]=0;
        }
    }
    else{
        FILE *fileptr=fopen(fileName,"rb");//open the file
        fread(&n,sizeof(int),1,fileptr);//read stuff
        fread(&playerCount,sizeof(int),1,fileptr);
        fread(&playerIndex,sizeof(int),1,fileptr);
        fread(&aiCount,sizeof(int),1,fileptr);
        fread(&aiIndex,sizeof(int),1,fileptr);
        fread(&iStart,sizeof(int),1,fileptr);
        fread(&jStart,sizeof(int),1,fileptr);
        colorPoints=(int**)calloc(playerCount+aiCount,sizeof(int*));//allocate memory
        board=(int**)calloc(n,sizeof(int*));
        scores=(int*)calloc(playerCount+aiCount,sizeof(int));
        for(i=0;i<n;i++){
            board[i]=(int*)calloc(n,sizeof(int));//allocate memory
            fread(board[i],sizeof(int),n,fileptr);//read the board from the file
        }
        for(i=0;i<playerCount+aiCount;i++){
            colorPoints[i]=(int*)calloc(5,sizeof(int));//allocate the memory
            fread(colorPoints[i],sizeof(int),5,fileptr);
        }
        fclose(fileptr);
    }
    while(!checkIfTheGameIsOver(board,n)){
        while(playerIndex<playerCount&&!checkIfTheGameIsOver(board,n)){
            drawBord(board,n,colorPoints,playerCount,aiCount,scores);
            printf("%d. oyuncunun sirasi oynancak tasin i ve j sini girin oyunu kaydedip cikmak icin -1 girin\n",playerIndex+1);
            if(iStart==iTarget&&jStart==jTarget){//both istart and itarget are set to -2 at the start and at the end of every succesfull move the only time they wont be different is if there was a previous istart from a saved game
                while(findTheNumberOfMovesForAPiece(board,n,iStart,jStart)==0){//player selecting the piece
                    scanf("%d",&iStart);
                    if(iStart==-1){
                        printf("Kaydedilmesi istediginiz dosyanin ismini girin\n");
                        scanf("%s",newFileName);//need to check if the filename is valid
                        saveGame(board,colorPoints,n,playerCount,playerIndex,aiCount,aiIndex,-2,-2,newFileName);
                        return;
                    }
                    scanf("%d",&jStart);
                    printf("Sectiginiz konumda hamle yoktur lutfen baska bir yer secin\n");//cause drawBoard resets the terminal i can print this and it wont be seen if the selected position has moves
                }
            }
            
            while(!playerIsDone){//player moving the piece
                drawBord(board,n,colorPoints,playerCount,aiCount,scores);
                printf("%d. oyuncunun sirasi oynancak tasin i'si %d j'si %d tasin oynancagi yerin i ve j sini girin oyunu kaydedip cikmak icin -1 girin\n",playerIndex+1,iStart,jStart);
                scanf("%d",&iTarget);
                if(iTarget==-1){
                    printf("Kaydedilmesi istediginiz dosyanin ismini girin\n");
                    scanf("%s",newFileName);//need to check if the filename is valid
                    saveGame(board,colorPoints,n,playerCount,playerIndex,aiCount,aiIndex,iStart,jStart,newFileName);
                    return;
                }
                scanf("%d",&jTarget);

                if(iTarget<n&&iTarget>0&&jTarget<n&&jTarget>0)tempChip=board[(iStart+iTarget)/2][(jStart+jTarget)/2];
                if(makeMove(board,n,colorPoints,playerCount+aiCount,playerIndex+aiIndex,iStart,jStart,iTarget,jTarget)){
                    scores[playerIndex]=findMin(colorPoints[playerIndex],5);//calculate the score of the player
                    drawBord(board,n,colorPoints,playerCount,aiCount,scores);
                    printf("Hamle yapmamak icin 0 Devam etmek icin 1 hamleyi geri almak icin 2 yazin\n");
                    scanf("%d",&i);
                    if(i==0){
                        playerIsDone=1;
                        iStart=-2;//reset the i and j so that the new player starts fresh
                        jStart=-2;
                        iTarget=-2;
                        jTarget=-2;
                    }
                    else if(i==1&&!findTheNumberOfMovesForAPiece(board,n,iTarget,jTarget)){
                        printf("Daha fazla hamle yoktur devam etmek icin entera basin");
                        iStart=-2;
                        jStart=-2;
                        iTarget=-2;
                        jTarget=-2;
                        getchar();
                        playerIsDone=1;
                    }
                    else if(i==2){
                        colorPoints[playerIndex][tempChip-1]--;//remove the point from player
                        board[(iStart+iTarget)/2][(jStart+jTarget)/2]=tempChip ;//set it back to temp chip
                        board[iStart][jStart]=board[iTarget][jTarget];//copy the target piece to starting place
                        board[iTarget][jTarget]=0;//remove the target piece
                        scores[playerIndex]=findMin(colorPoints[playerIndex],5);//calculate the score of the player
                        drawBord(board,n,colorPoints,playerCount,aiCount,scores);
                    }
                    if(i!=2){//it shouldnt change istart if the move was reversed
                        iStart=iTarget;//this part isnt tested properly
                        jStart=jTarget;
                    }
                }
                
            }
           playerIsDone=0;
           playerIndex++;
        }
        
        while(aiIndex<aiCount&&!checkIfTheGameIsOver(board,n)){//ai part
            moveStruct* tempMove,*move;
            drawBord(board,n,colorPoints,playerCount,aiCount,scores);
            printf("ai%d starting their turn press enter to continue\n",aiIndex+1);//this helps with the debugging
            getchar();
            move=findPathsOnTheWholeBoard(board,n,colorPoints[playerCount+aiIndex],colorPoints[(playerCount+aiIndex+1)%(playerCount+aiCount)]);
            tempMove=move;//save the first move differently so that we can free it up later
            printf("ai%d devam etmek icin entera basin\n",aiIndex+1);
            while(tempMove->depth >0){
                i=tempMove->i;//save the starting i and j
                j=tempMove->j;
                tempMove=selectPath(tempMove);//select the best path
                makeMove(board,n,colorPoints,playerCount+aiCount,playerIndex+aiIndex,i,j,tempMove->i,tempMove->j);//make the move
                scores[playerCount+aiIndex]=findMin(colorPoints[playerCount+aiIndex],5);//calculate the score of the player
                drawBord(board,n,colorPoints,playerCount,aiCount,scores);
                //printf("enemy score %d enemy depth %d\nai%d devam etmek icin entera basin\n",tempMove->enemyScore,tempMove->enemyDepth,aiIndex+1);
                getchar();
            }
            freeMoveStruct(move);
            aiIndex++;
        }
        aiIndex=0;
        playerIndex=0;
    }
    drawBord(board,n,colorPoints,playerCount,aiCount,scores);
   for(i=0;i<playerCount+aiCount;i++)
    {
        if(scores[i]>maxRows){
            maxRows=scores[i];
            maxExtraSkippers=sumOfTheList(colorPoints[i],5)-5*scores[i];//extra skippers calculation
            maxPointsIndex=i;
        }
        else if(scores[i]==maxRows &&sumOfTheList(colorPoints[i],5)-5*scores[i]>maxExtraSkippers){
            maxExtraSkippers=sumOfTheList(colorPoints[i],5)-5*scores[i];//extra skippers calculation
            maxPointsIndex=i;
        }
    }
    printf("kazanan %s %d devam etmek icin entera basin\n",maxPointsIndex>=playerCount? "Ai":"Player",maxPointsIndex>=playerCount? maxPointsIndex-playerCount+1:maxPointsIndex+1);
    getchar();
}
int menu(){
    int input;
    
    printf("1: Yeni oyun\n2: Kalan oyuna devam et\n3: cikis\n");
    scanf("%d",&input);
    return input;
}
int main(){
    srand(time(NULL));//initialize random
    int input=0;
    char gameFileName[100];
    while(input!=3){
        input =menu();
        if(input ==1)game(NULL);
        else if(input==2){
            printf("Kayit dosyasinin ismini girin\n");
            scanf("%s",gameFileName);
            game(gameFileName);
        }
    }
}