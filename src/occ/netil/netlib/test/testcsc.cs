namespace a.b.c
{
	public class r
	{
		public class s
		{
			private static int _myprop;
			public static int zz = 5;
			public static int myprop {
				get {
					return _myprop + 11;
				}
				set {
					_myprop = value + 22;
				}
			}
		}
	}
	
};