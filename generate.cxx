#include <iostream>
#include <vector>
#include <string>
#include <map>

#define NUMBER_LETTERS 26

using LetterCounts = std::vector<int>;

inline bool increment(LetterCounts& node, LetterCounts& start_node, LetterCounts& end_node) {

    for(unsigned int ind = 0; ind < node.size(); ind++) {
	if(node[ind] < end_node[ind]) {
	    // can still increment the LSB without overflow
	    node[ind]++;
	    return true;
	}
	else {
	    // LSB will wrap around to the beginning, need to propagate carry
	    node[ind] = start_node[ind];
	}
    }
    return false;
}

LetterCounts letter_count(std::string input) {
    
    LetterCounts letter_count(NUMBER_LETTERS);

    for(const char& cur_char: input) {
	if(cur_char >= 'a' && cur_char <= 'z') {
	    letter_count[cur_char - 'a']++;
	}
    }

    return letter_count;
}

inline void set_letter_count(char letter, int count, LetterCounts& letter_counts) {
    letter_counts[letter - 'a'] = count;
}

inline int get_letter_count(char letter, LetterCounts& letter_counts) {
    return letter_counts[letter - 'a'];
}

void sum_letter_counts(std::vector<LetterCounts>& summands, LetterCounts& sum) {
    
    // reset the counters
    std::fill(sum.begin(), sum.end(), 0);

    // perform the sums
    for(unsigned int ind = 0; ind < sum.size(); ind++) {
	
	int cur_sum = 0;
	for(LetterCounts& cur_summand: summands) {
	    cur_sum += cur_summand[ind];
	}
	sum[ind] = cur_sum;
    }
}

void print_letter_counts(LetterCounts letter_counts) {
    
    for(unsigned int ind = 0; ind < letter_counts.size(); ind++) {
	std::cout << (char)(ind + 'a') << " : " << letter_counts[ind] << std::endl;
    }
}

int main(void) {

    // ------------------------------
    // database with number words
    // ------------------------------
    std::map<unsigned int, std::string> num2word = {
	{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"}, {6, "six"}, {7, "seven"},
	{8, "eight"}, {9, "nine"}, {10, "ten"}, {11, "eleven"}, {12, "twelve"}, {13, "thirteen"},
	{14, "fourteen"}, {15, "fifteen"}, {16, "sixteen"}, {17, "seventeen"}, {18, "eighteen"},
	{19, "nineteen"}, {20, "twenty"}, {21, "twenty-one"}, {22, "twenty-two"}, {23, "twenty-three"},
	{24, "twenty-four"}, {25, "twenty-five"}, {26, "twenty-six"}, {27, "twenty-seven"}, 
	{28, "twenty-eight"}, {29, "twenty-nine"}, {30, "thirty"}, {31, "thirty-one"},
	{32, "thirty-two"}, {33, "thirty-three"}, {34, "thirty-four"}, {35, "thirty-five"},
	{36, "thirty-six"}, {37, "thirty-seven"}, {38, "thirty-eight"}, {39, "thirty-nine"},
	{40, "fourty"}
    };

    // ------------------------------
    // ranges for the exploration of certain characters
    // ------------------------------
    std::map<char, unsigned int> input_start_frequencies = {
	{'e', 28}, {'f', 7}, {'g', 2}, {'h', 4}, {'i', 10}, {'l', 2}, {'n', 21}, {'o', 14}, {'r', 6},
	{'s', 25}, {'t', 18}, {'u', 3}, {'v', 4}, {'w', 8}, {'x', 1}, {'y', 3}
    };
    std::map<char, unsigned int> input_end_frequencies = {
	{'e', 30}, {'f', 9}, {'g', 4}, {'h', 6}, {'i', 12}, {'l', 4}, {'n', 23}, {'o', 16}, {'r', 8},
	{'s', 27}, {'t', 20}, {'u', 5}, {'v', 6}, {'w', 10}, {'x', 3}, {'y', 5}
    };

    // std::map<char, unsigned int> input_start_frequencies = {
    // 	{'e', 25}, {'f', 4}, {'g', 2}, {'h', 3}, {'i', 8}, {'l', 1}, {'n', 17}, {'o', 12}, {'r', 3},
    // 	{'s', 24}, {'t', 18}, {'u', 1}, {'v', 3}, {'w', 7}, {'x', 1}, {'y', 3}
    // };
    // std::map<char, unsigned int> input_end_frequencies = {
    // 	{'e', 32}, {'f', 9}, {'g', 7}, {'h', 8}, {'i', 14}, {'l', 4}, {'n', 23}, {'o', 17}, {'r', 8},
    // 	{'s', 30}, {'t', 24}, {'u', 6}, {'v', 8}, {'w', 13}, {'x', 6}, {'y', 5}
    // };

    // ------------------------------
    // initial part of this pangram
    // ------------------------------
    std::string sentence_prefix = "this pangram lists and";

    // ------------------------------
    // build database of letter counts
    // ------------------------------
    // for the sentence prefix
    LetterCounts sentence_prefix_letter_counts = letter_count(sentence_prefix);

    // for the number words
    std::map<unsigned int, LetterCounts> number_word_letter_counts;
    for(auto& cur: num2word) {
	unsigned int cur_number = cur.first;
	std::string cur_word = cur.second;

	LetterCounts cur_letter_counts = letter_count(cur_word);

	// special case: if the number is not "one", need to absorb the plural 's' into the library also
	if(cur_number != 1) {
	    set_letter_count('s', get_letter_count('s', cur_letter_counts) + 1, cur_letter_counts);
	}
	number_word_letter_counts[cur_number] = cur_letter_counts;
    }

    // ------------------------------
    // fix the counts of letters that can be fixed already
    // ------------------------------
    // list of characters that don't occur in any number word and whose counts can thus be determined directly
    std::vector<char> direct_characters = {'a', 'b', 'c', 'd', 'j', 'k', 'm', 'p', 'q', 'z'};

    for(auto& cur_character: direct_characters) {
	unsigned int needed_frequency = get_letter_count(cur_character, sentence_prefix_letter_counts) + 1;
	input_start_frequencies[cur_character] = needed_frequency;
	input_end_frequencies[cur_character] = needed_frequency;
    }

    std::cout << "library complete" << std::endl;

    // ------------------------------
    // load everything into the right position
    // ------------------------------
    // prepare the start and end points of the search
    LetterCounts start_frequencies(NUMBER_LETTERS);
    LetterCounts end_frequencies(NUMBER_LETTERS);

    // fill in the start and end frequencies from the library
    for(auto& cur: input_start_frequencies) {
	char cur_letter = cur.first;
	unsigned int cur_frequency = cur.second;
	set_letter_count(cur_letter, cur_frequency, start_frequencies);
    }

    for(auto& cur: input_end_frequencies) {
	char cur_letter = cur.first;
	unsigned int cur_frequency = cur.second;
	set_letter_count(cur_letter, cur_frequency, end_frequencies);
    }

    std::cout << "setup complete" << std::endl;

    std::cout << "- - - - - - - - - - - - - - -" << std::endl;
    std::cout << "START COUNTS" << std::endl;
    print_letter_counts(start_frequencies);
    std::cout << "- - - - - - - - - - - - - - -" << std::endl;

    std::cout << "- - - - - - - - - - - - - - -" << std::endl;
    std::cout << "END COUNTS" << std::endl;
    print_letter_counts(end_frequencies);
    std::cout << "- - - - - - - - - - - - - - -" << std::endl;

    // ------------------------------
    // perform the search
    // ------------------------------
    LetterCounts cur_frequencies = start_frequencies;
    LetterCounts obtained_frequencies(NUMBER_LETTERS);
    bool successful = false;
    unsigned long long cnt = 0;
    do {

	// compute the character counts implied by this assignment
	std::vector<LetterCounts> cur_letter_counts = {sentence_prefix_letter_counts};
	for(unsigned int cur_count: cur_frequencies) {
	    cur_letter_counts.push_back(number_word_letter_counts[cur_count]);
	}
	sum_letter_counts(cur_letter_counts, obtained_frequencies);

	// the letters themselves are also part of the enumeration
	for(auto& cur_count: obtained_frequencies) {
	    cur_count += 1;
	}

	if(obtained_frequencies == cur_frequencies) {
	    // have found a fixed point!
	    successful = true;
	    break;
	}

	cnt++;
	if(cnt % 1000000 == 0) {
	    std::cout << "have checked " << cnt << " combinations" << std::endl;
	}
    }
    while(increment(cur_frequencies, start_frequencies, end_frequencies));

    // ------------------------------
    // compose the final pangram, if one exists
    // ------------------------------
    if(successful) {
	std::string pangram = sentence_prefix;

	// read them in alphabetical order
	std::vector<char> all_characters = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
	
	// do the composing
	for(auto& cur_character: all_characters) {
	    unsigned int cur_frequency = get_letter_count(cur_character, cur_frequencies);
	    std::string number_word = num2word[cur_frequency];
	    
	    if(cur_frequency == 1) {
		pangram += " " + number_word + " " + cur_character;
	    }
	    else if(cur_frequency > 1) {
		pangram += " " + number_word + " " + cur_character + "'s";
	    }
	}

	std::cout << "PANGRAM FOUND!" << std::endl;
	std::cout << pangram << std::endl;
    }
    else {
	std::cout << "NO PANGRAM FOUND!" << std::endl;
    }

    return 0;
}
