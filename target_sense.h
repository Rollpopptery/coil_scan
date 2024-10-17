#ifndef _TARGET_SENSE_H
#define _TARGET_SENSE_H


// points on the sample array and where they are in time.
//
#define INDEX_12uSEC (4)
#define INDEX_15uSEC (5)
#define INDEX_18uSEC (6)
#define INDEX_21uSEC (7)
#define INDEX_24uSEC (8)
#define INDEX_27uSEC (9)
#define INDEX_30uSEC (10)
#define INDEX_33uSEC (11)
#define INDEX_36uSEC (12)
#define INDEX_39uSEC (13)
#define INDEX_42uSEC (14)
#define INDEX_45uSEC (15)
#define INDEX_48uSEC (16)
#define INDEX_51uSEC (17)
#define INDEX_54uSEC (18)
#define INDEX_57uSEC (19)
#define INDEX_60uSEC (20)
#define INDEX_63uSEC (21)
#define INDEX_66uSEC (22)
#define INDEX_69uSEC (23)


// samples on the discharge curve
//
#define TIME_POINTS (50)

int SAMPLE_INDEXES_SET1[TIME_POINTS];

void normalise(double arr[], int sz);
bool isTarget_Set1(double sig_curve[]);
bool isIron_Set1(double sig_curve[], bool printOut );
void targetID_Set1(double sig_curve[], bool printOut);

//
enum TARGETID{OK_BIG, OK_SMALL, Fe, NO_TARGET};

class TARGET_SENSE{

  public:
    static TARGETID targetID;          
};

TARGETID TARGET_SENSE::targetID;



#endif
// _TARGET_SENSE_H