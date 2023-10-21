#include "../../include/types.h"
#include "../../include/reader.h"

int main(void) {
    InputBuffer * input_buffer = new_input_buffer();
    set_input_buffer(input_buffer, "()");;
    TokenReader * token_reader = tokenize(input_buffer);
    
    free_reader(token_reader);
    free_input_buffer(input_buffer);
}
