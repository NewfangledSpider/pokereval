#include <stdio.h>

#include "poker_defs.h"
#include "mktable.h"

#define CM_COMMENT_STRING \
 "StdDeck_cardMasks[].  Maps card indices (0..51) to CardMasks.  \n"         \
 "The output mask has only one bit set, the bit corresponding to the card\n" \
 "identified by the index." 
#define CM_FILENAME "t_cardmasks"

#define TB_COMMENT_STRING \
 "topBitTable[].  Maps 13-bit rank masks to a 13-bit rank mask.\n"           \
 "The output mask has only one bit set, the bit corresponding to the\n"      \
 "highest bit which was set in the input mask. "
#define TB_FILENAME "t_topbit"

#define TC_COMMENT_STRING \
 "topCardTable[].  Maps 13-bit rank masks to an integer corresponding to\n"  \
 "a card rank.  The output value is the rank of the highest card set \n"     \
 "in the input mask.  "
#define TC_FILENAME "t_topcard"

#define T5C_COMMENT_STRING \
 "topFiveCardsTable[].  Maps 13-bit rank masks to a HandVal structure with\n" \
 "the xxx_bit fields set.  The fields correspond to the rank values of the\n" \
 "top five bits set in the input rank mask. "
#define T5C_FILENAME "t_topfivecards"

#define NB_COMMENT_STRING \
 "nBitsTable[].  Maps 13-bit rank masks to the number of bits that are set\n" \
 "in the mask. "
#define NB_FILENAME "t_nbits"

#define ST_COMMENT_STRING \
 "straightTable[].  Maps 13-bit rank masks to a value indicating if a \n"    \
 "straight is present, and if so, the rank of the high card.  Zero means\n"  \
 "no straight, even though zero corresponds to deuce, but since there\n"     \
 "is no such thing as a deuce-high straight, that's OK. "
#define ST_FILENAME "t_straight"

static void 
doNBitsTable(void) {
  int i;

  MakeTable_begin("nBitsTable", 
                  NB_FILENAME, 
                  "uint8", 
                  StdDeck_N_RANKMASKS);
  MakeTable_comment(NB_COMMENT_STRING);
  for (i=0; i < StdDeck_N_RANKMASKS; i++) 
    MakeTable_outputUInt8(n_bits_func(i));

  MakeTable_end();
}


static void 
doTopCardTable(void) {
  int i;

  MakeTable_begin("topCardTable", 
                  TC_FILENAME, 
                  "uint8", 
                  StdDeck_N_RANKMASKS);
  MakeTable_comment(TC_COMMENT_STRING);
  for (i=0; i < StdDeck_N_RANKMASKS; i++) 
    MakeTable_outputUInt8(top_card_func(i));

  MakeTable_end();
}


static void 
doTopBitTable(void) {
  int i;

  MakeTable_begin("topBitTable", 
                  TB_FILENAME, 
                  "uint32", 
                  StdDeck_N_RANKMASKS);
  MakeTable_comment(TB_COMMENT_STRING);
  for (i=0; i < StdDeck_N_RANKMASKS; i++) 
    MakeTable_outputUInt32(top_bit_func(i));

  MakeTable_end();
}


static void 
doTopFiveCardsTable(void) {
  int i;

  MakeTable_begin("topFiveCardsTable", 
                  T5C_FILENAME, 
                  "uint32", 
                  StdDeck_N_RANKMASKS);
  MakeTable_comment(T5C_COMMENT_STRING);
  for (i=0; i < StdDeck_N_RANKMASKS; i++) {
    StdRules_HandVal eval;
    int n = i;

    eval.handval_n = 0;
    eval.handval.top_card    = top_card_func(n);
    n &= ~(1 << eval.handval.top_card);
    eval.handval.second_card = top_card_func(n);
    n &= ~(1 << eval.handval.second_card);
    eval.handval.third_card  = top_card_func(n);
    n &= ~(1 << eval.handval.third_card);
    eval.handval.fourth_card = top_card_func(n);
    n &= ~(1 << eval.handval.fourth_card);
    eval.handval.fifth_card  = top_card_func(n);

    MakeTable_outputUInt32(eval.handval_n);
  };

  MakeTable_end();
}


static void 
doStraightTable(void) {
  int i;

  MakeTable_begin("straightTable", 
                  ST_FILENAME, 
                  "uint8", 
                  StdDeck_N_RANKMASKS);
  MakeTable_comment(ST_COMMENT_STRING);
  for (i=0; i < StdDeck_N_RANKMASKS; i++) {
    uint32 ranks, ranks2, val;

    ranks = i;
    val = 0;
    if ( (ranks2  = ranks & (ranks << 1)) &&
         (ranks2 &=         (ranks << 2)) &&
         (ranks2 &=         (ranks << 3)) &&
         (ranks2 &=         (ranks << 4)) ) {
      val = top_card_func(ranks2);
    } 
    else if ((ranks & StdRules_FIVE_STRAIGHT) == StdRules_FIVE_STRAIGHT) 
      val = StdDeck_Rank_5;

    MakeTable_outputUInt8(val);
  };

  MakeTable_end();
}


static void 
doCardMaskTable(void) {
  StdDeck_CardMask c;
  int i;
  char buf[80];

  MakeTable_begin("StdDeck_cardMasksTable", 
                  CM_FILENAME, 
                  "StdDeck_CardMask", 
                  StdDeck_N_CARDS);
  MakeTable_comment(CM_COMMENT_STRING);
  for (i=0; i<StdDeck_N_CARDS; i++) {
    int suit = StdDeck_SUIT(i);
    int rank = StdDeck_RANK(i);

    StdDeck_CardMask_RESET(c);
    if (suit == StdDeck_Suit_HEARTS)
      c.cards.hearts = (1 << rank);
    else if (suit == StdDeck_Suit_DIAMONDS)
      c.cards.diamonds = (1 << rank);
    else if (suit == StdDeck_Suit_CLUBS)
      c.cards.clubs = (1 << rank);
    else if (suit == StdDeck_Suit_SPADES)
      c.cards.spades = (1 << rank);

#if HAVE_INT64
    MakeTable_outputUInt64(c.cards_n);
#else
    sprintf(buf, " { { 0x%08x, 0x%08x } } ", c.cards_nn.n1, c.cards_nn.n2);
    MakeTable_outputString(buf);
#endif
  };

  MakeTable_end();
}
    
int 
main(int argc, char **argv) {
  doCardMaskTable();
  doNBitsTable();
  doTopCardTable();
  doTopBitTable();
  doTopFiveCardsTable();
  doStraightTable();

  return 0;
}

