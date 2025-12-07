using DotPython;


namespace DotPython.Tests
{
    [TestClass]
    public sealed class BasicTests
    {
        private static PyContext _pyContext;

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            Python.Initialize();
            _pyContext = new PyContext();
        }
        [ClassCleanup]
        public static void ClassCleanup()
        {
            _pyContext.Dispose();
        }

        [TestMethod]
        public void InvokeMemberTest()
        {

            var math = _pyContext.Import("math");
            var cosine = math.cos(2);
            
        }
        

        


    }
}
