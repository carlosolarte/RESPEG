/* expected '(' after "while" */

int main(){
    while true {
        puts("Hello, World");
        break;
    }
}

/*OK:
int main(){
    while(true){
        puts("Hello, World");
        break;
    }
}
*/
