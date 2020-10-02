using System.Threading;

    
public struct C2SimpleSpinLock
{
    int flag;

    public C2SimpleSpinLock(int n = 0)
    {
        this.flag = 0;
    }

    public void Enter()
    {
        SpinWait wait = new SpinWait();

        for (; 0 != flag || 0 != Interlocked.Exchange(ref flag, 1);)
        {
            wait.SpinOnce();
        }
    }

    public void Leave()
    {
        Interlocked.Exchange(ref flag, 0);
    }

}
