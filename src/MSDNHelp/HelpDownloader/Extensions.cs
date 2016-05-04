namespace VisualStudioHelpDownloader2012
{
	using System;
	using System.Collections.Generic;

	/// <summary>
	///     Help class with extension methods.
	/// </summary>
	internal static class Extensions
	{
		/// <summary>
		/// Perform action on each element from source.
		/// </summary>
		/// <typeparam name="T">
		/// Element type.
		/// </typeparam>
		/// <param name="source">
		/// Elements source.
		/// </param>
		/// <param name="action">
		/// Action to perform.
		/// </param>
		public static void ForEach<T>( this IEnumerable<T> source, Action<T> action )
		{
			if ( null != action )
			{
				foreach ( T x in source )
				{
					action( x );
				}
			}
		}
	}
}
