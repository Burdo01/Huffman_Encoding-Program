//
// STARTER CODE: util.h
//
// Program secures and shrinks text bit size by encoding and compressing files
// frequncy map is built based on characters
// encoding tree is then built from map
// encoding map is built from encoding tree and uses helper to do so
// file is then encoded using the encoding map and output returned as string
// decode then traverses the tree given the encoded input and root of encoding tree 
// and outputs decoded characters to string
// compress puts previous functions togethor and builds and encoded .txt.huf file
// decompress then reverses the compress function and outputs decode _unc.txt file

#pragma once

// TODO: includes go here

typedef hashmap hashmapF;
typedef unordered_map<int, string> hashmapE;

struct HuffmanNode
{
    int character;
    int count;
    HuffmanNode *zero;
    HuffmanNode *one;
};

struct compare
{
    bool operator()(const HuffmanNode *lhs,
                    const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode *node)
{
    // recursivley free tree and delete nodes
    if (node == NULL)
    {
        return;
    }

    freeTree(node->zero);
    freeTree(node->one);

    delete node;
    node = nullptr;
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map)
{

    // if true file parsing creating map of char count
    if (isFile == true)
    {
        char c;

        fstream ifs(filename);
        // check file
        if (!ifs.is_open())
        {
            cout << filename << " HAS FAILED TO OPEN!" << endl;
        }
        // parse file by char and place in map
        if (ifs.is_open())
        {
            // while (ifs.get(c))
            while (!ifs.eof())
            {
                ifs.get(c);
                int i = 0;
                if (map.containsKey(c) == true)
                {
                    i = map.get(c);
                    i++;
                    map.put(c, i);
                }
                else
                {
                    map.put(c, 1);
                }
            }
            map.put(PSEUDO_EOF, 1);
        }
        ifs.close();
    }
    // parse through string and adds values and frequencies into map
    else
    {
        for (int x : filename)
        {
            int j = 0;
            if (map.containsKey(x) == true)
            {
                j = map.get(x);
                j++;
                map.put(x, j);
            }
            else
            {
                map.put(x, 1);
            }
        }
        map.put(PSEUDO_EOF, 1);
    }
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode *buildEncodingTree(hashmapF &map)
{
    // sorts by lowest priority at top using vector comparison functor
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, compare> pqc;

    if (map.size() == 0)
    {
        return nullptr;
    }

    // iterates through map keys and build map saving keys and count
    for (int x : map.keys())
    {
        HuffmanNode *node = new HuffmanNode;
        node->character = x;
        node->count = map.get(x);
        node->zero = nullptr;
        node->one = nullptr;
        pqc.push(node);
        // cout << pqc.size() << endl; // size check
    }
    // int j = 0; // for value check

    while (pqc.size() != 1)
    {
        HuffmanNode *node2 = new HuffmanNode;
        // n1 stores and keeps track of count value for addition in function
        HuffmanNode *n1 = pqc.top();
        node2->zero = n1;
        // value check
        // cout << "count " << j << " = " << n->count << ", char: " << n->character << endl;
        pqc.pop();

        // n2 stores and keeps track of count value for addition in function
        HuffmanNode *n2 = pqc.top();
        node2->one = n2;
        // value check
        // cout << "count " << j << " = " << n2->count << ", char: " << n2->character << endl;
        // j++;
        pqc.pop();

        int i = n1->count + n2->count;
        node2->count = i;
        node2->character = NOT_A_CHAR;
        // value check for binary tree
        // cout << "count = " << i << endl;
        pqc.push(node2);
    }
    // size verification
    // cout << "Size of pqc = " << pqc.size() << endl;
    // HuffmanNode* root = pqc.top();

    return pqc.top(); // root node
}

//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode *node, hashmapE &encodingMap, string str,
                       HuffmanNode *prev)
{

    //iterate through tree recursivley till found value
    if (node->character != NOT_A_CHAR)
    {
        encodingMap[node->character] = str;
        return;
    }

    _buildEncodingMap(node->zero, encodingMap, str + "0", prev);
    // remove last 0
    str.substr(str.length(), -1);
    _buildEncodingMap(node->one, encodingMap, str + "1", prev);
    // remove last 1
    str.substr(str.length(), -1);
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode *tree)
{
    hashmapE encodingMap;
    HuffmanNode *prev = NULL;
    string str = "";

    _buildEncodingMap(tree, encodingMap, str, prev);

    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream &input, hashmapE &encodingMap, ofbitstream &output,
              int &size, bool makeFile)
{
    char c;
    string s = "";
    string outputStr = "";

     // map parsing bits
    if (makeFile == true)
    {
        while (input.get(c))
        {
            s = encodingMap.at(c);

            outputStr += s;
            for (unsigned int i = 0; i < s.size(); i++)
            {
                int x = s[i];

                if (x == 48)
                {
                    output.writeBit(0);
                }
                else
                {
                    output.writeBit(1);
                }
            }
        }
        // input of PSUEDO_EOF to string to add at end of output
        string st = encodingMap.at(PSEUDO_EOF);
        outputStr += st;

        // Writing PSUEDO_EOF bits from string
        for (unsigned int i = 0; i < st.size(); i++)
        {
            int x = st[i];

            if (x == 48)
            {
                output.writeBit(0);
            }
            else
            {
                output.writeBit(1);
            }
        }
        // output << str; // old output, not correct route

        // output << encodingMap.at(PSEUDO_EOF); // old
    }

    size = outputStr.size();
    return outputStr; // TO DO: update this return
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode *encodingTree, ofstream &output)
{
    string str = "";
    HuffmanNode *cur = encodingTree;

    while (!input.eof())
    {
        if (cur->character == PSEUDO_EOF || cur->character == NULL)
        {
            break;
        }
        else if (cur->character != NOT_A_CHAR)
        {
            output << cur->character;
            str += cur->character;
            cur = encodingTree;
        }
        else if (input.readBit() == 0)
        {
            cur = cur->zero;
        }
        else
        {
            cur = cur->one;
        }
        // same as above but different check
        /*if (cur->zero == nullptr and cur->one == nullptr)
        {
            output << cur->character;
            s += cur->character;
            cur = encodingTree;
        }*/
        // alternate decoding form
        /* string k = "";
         input >> k;
         for (int i = 0; i < k.size(); i++)
         {
            if (k[i] == '0')
            cur = cur->zero;
           else
            cur = cur->one;

          if (cur->zero == NULL and cur->one == NULL)
          {
            s += cur->character;
            cur = encodingTree;
          }
         }*/
    }

    return str;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename)
{
    hashmapF map;
    HuffmanNode *root;
    hashmapE encodingMap;
    ifstream input(filename);
    int size = 0;

    buildFrequencyMap(filename, true, map);
    root = buildEncodingTree(map);
    encodingMap = buildEncodingMap(root);

    ofbitstream output(filename + ".huf");
    output << map;

    string codeStr = encode(input, encodingMap, output, size, true);

    freeTree(root);

    return codeStr;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; '
//
// (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename)
{
    hashmapF map;

    // load .txt.huf file
    ifbitstream input(filename);
    // insert input back into map form
    input >> map;

    // build encoding map from .txt file
    string fn1 = filename.substr(0, filename.size() - 4);
    HuffmanNode *root = buildEncodingTree(map);

    ofstream outputParam;
    string decoded = decode(input, root, outputParam);

    // edit string and insert decoded text
    string fn2 = filename.substr(0, filename.size() - 8);
    ofstream outputFinal(fn2 + "_unc.txt");
    outputFinal << decoded;

    freeTree(root);

    return decoded;
}