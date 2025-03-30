
#if _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif

#include "Minigin.h"

void load()
{
}

int main(int argc, char* argv[]) {
    dae::Minigin engine("../Data/");
    engine.Run(load);
    return 0;
}
