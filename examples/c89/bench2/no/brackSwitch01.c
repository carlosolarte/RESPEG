/* expected '(' after "switch" */

int main(){
    int x = 1;
    switch x{
        case 1:
        break;
    }
}

/*OK:
int main(){
    int x = 1;
    switch(x){
        default:
        break;
    }
}
*/
