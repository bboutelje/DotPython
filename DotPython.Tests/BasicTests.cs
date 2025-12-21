using DotPython;
using System;
using System.Reflection.PortableExecutable;
using System.Runtime.Remoting.Contexts;


namespace DotPython.Tests
{
    [TestClass]
    public sealed class BasicTests
    {
        private static PyContext? _pyContext;

        [ClassInitialize]
        public static void ClassInitialize(TestContext context)
        {
            _pyContext = new PyContext();
            _pyContext.AddToSysPath(AppDomain.CurrentDomain.BaseDirectory);
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            _pyContext!.Dispose();
        }

        [TestMethod]
        public void TryInvokeMemberTest()
        {
            var math = _pyContext!.Import("math");
            var pi = math.pi;
            var cosine = math.cos(2);
        }

        [TestMethod]
        public void ListTest()
        {
            var builtins = _pyContext!.Import("builtins");
            var list1 = builtins.list(new int[] { 1, 2, 3, 4, 5 });
            list1.sort(reverse: true);

            for (int i = 0; i < 5; i++)
            {
                Assert.AreEqual(5 - i, list1[i]);
            }
        }

        [TestMethod]
        public void ImportModuleTest()
        {
            var helloApp = _pyContext!.Import("hello");
            Assert.AreEqual("Hello, Python programmer!", helloApp.greet_user());
        }

        [TestMethod]
        public void GetAttrTest()
        {
            var sys = _pyContext!.Import("sys");
            var version = sys.GetAttr("version");
            Assert.IsNotNull(version);
        }

        [TestMethod]
        public void DictionaryTest()
        {
            var dict = new Dictionary<string, int>()
            {
                { "a", 1 },
                { "b", 2 },
                { "c", 3 }
            };

            var pyDict = dict.ToPython();
            pyDict["a"] = 9;
            Assert.AreEqual(9, pyDict["a"]);

        }

        [TestMethod]
        public void BinaryOptionTest()
        {
            var a = new[] { 0, 1, 2, 3 }.ToPython();
            var c = a[3] * a[3];
            Assert.AreEqual(9, c);
        }

        [TestMethod]
        public void NugetDocExampleTest()
        {
            var np = _pyContext!.Import("numpy");

            var arr = np.array(new[] { 0, 1, 2 }, dtype: np.int32);
            var mean = arr.mean();
            Assert.AreEqual(1.0, mean);

            
        }

        [TestMethod]
        public void IEnumerableTest()
        {
            var range = Enumerable.Range(0, 5);
            
            var builtins = _pyContext!.Import("builtins");
            var sum = builtins.sum(range);

            Assert.AreEqual(10, sum);
        }

        [TestMethod]
        public void HashSetTest()
        {
            var hashSet = new HashSet<string>() { "apple", "banana", "cherry" };
            var builtins = _pyContext!.Import("builtins");
            var pythonSet = builtins.set(hashSet);
            hashSet.Remove("banana");
            Assert.AreEqual(1, pythonSet.__contains__("apple"));
        }

    }
}
