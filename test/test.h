//
//  test.h
//  
//
//  Created by user on 7/29/14.
//
//

#ifndef ____test__
#define ____test__


class Test{
private:
    //测试参数
    char wavsrc[21];
    char scoresrc[21];
    //
    
public:
    enum MODE{
        WAV = 0,
        MP3
    };
    void _score2wav();
    void _wav2score();

    Test();
    ~Test();
};

#endif /* defined(____test__) */
